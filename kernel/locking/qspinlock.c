// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Queued spinlock
 *
 * (C) Copyright 2013-2015 Hewlett-Packard Development Company, L.P.
 * (C) Copyright 2013-2014,2018 Red Hat, Inc.
 * (C) Copyright 2015 Intel Corp.
 * (C) Copyright 2015 Hewlett-Packard Enterprise Development LP
 *
 * Authors: Waiman Long <longman@redhat.com>
 *          Peter Zijlstra <peterz@infradead.org>
 */

#ifndef _GEN_PV_LOCK_SLOWPATH

#include <linux/smp.h>
#include <linux/bug.h>
#include <linux/cpumask.h>
#include <linux/percpu.h>
#include <linux/hardirq.h>
#include <linux/mutex.h>
#include <linux/prefetch.h>
#include <asm/byteorder.h>
#include <asm/qspinlock.h>

/*
 * Include queued spinlock statistics code
 */
#include "qspinlock_stat.h"

/*
 * The basic principle of a queue-based spinlock can best be understood
 * by studying a classic queue-based spinlock implementation called the
 * MCS lock. A copy of the original MCS lock paper ("Algorithms for Scalable
 * Synchronization on Shared-Memory Multiprocessors by Mellor-Crummey and
 * Scott") is available at
 *
 * https://bugzilla.kernel.org/show_bug.cgi?id=206115
 *
 * This queued spinlock implementation is based on the MCS lock, however to
 * make it fit the 4 bytes we assume spinlock_t to be, and preserve its
 * existing API, we must modify it somehow.
 *
 * In particular; where the traditional MCS lock consists of a tail pointer
 * (8 bytes) and needs the next pointer (another 8 bytes) of its own node to
 * unlock the next pending (next->locked), we compress both these: {tail,
 * next->locked} into a single u32 value.
 *
 * Since a spinlock disables recursion of its own context and there is a limit
 * to the contexts that can nest; namely: task, softirq, hardirq, nmi. As there
 * are at most 4 nesting levels, it can be encoded by a 2-bit number. Now
 * we can encode the tail by combining the 2-bit nesting level with the cpu
 * number. With one byte for the lock value and 3 bytes for the tail, only a
 * 32-bit word is now needed. Even though we only need 1 bit for the lock,
 * we extend it to a full byte to achieve better performance for architectures
 * that support atomic byte write.
 *
 * We also change the first spinner to spin on the lock bit instead of its
 * node; whereby avoiding the need to carry a node from lock to unlock, and
 * preserving existing lock API. This also makes the unlock code simpler and
 * faster.
 *
 * N.B. The current implementation only supports architectures that allow
 *      atomic operations on smaller 8-bit and 16-bit data types.
 *
 */

#include "mcs_spinlock.h"
#define MAX_NODES	4

/*
 * On 64-bit architectures, the mcs_spinlock structure will be 16 bytes in
 * size and four of them will fit nicely in one 64-byte cacheline. For
 * pvqspinlock, however, we need more space for extra data. To accommodate
 * that, we insert two more long words to pad it up to 32 bytes. IOW, only
 * two of them can fit in a cacheline in this case. That is OK as it is rare
 * to have more than 2 levels of slowpath nesting in actual use. We don't
 * want to penalize pvqspinlocks to optimize for a rare case in native
 * qspinlocks.
 */
/*xiaojin-spinlock-qspinlock -1.1 qnode struct*/
struct qnode {
	struct mcs_spinlock mcs;
#ifdef CONFIG_PARAVIRT_SPINLOCKS
	long reserved[2];
#endif
};

/*
 * The pending bit spinning loop count.
 * This heuristic is used to limit the number of lockword accesses
 * made by atomic_cond_read_relaxed when waiting for the lock to
 * transition out of the "== _Q_PENDING_VAL" state. We don't spin
 * indefinitely because there's no guarantee that we'll make forward
 * progress.
 */
#ifndef _Q_PENDING_LOOPS
#define _Q_PENDING_LOOPS	1
#endif

/*
 * Per-CPU queue node structures; we can never have more than 4 nested
 * contexts: task, softirq, hardirq, nmi.
 *
 * Exactly fits one 64-byte cacheline on a 64-bit architecture.
 *
 * PV doubles the storage and uses the second cacheline for PV state.
 */
/*xiaojin-spinlock-qspinlock -1 DEFINE_PER_CPU_ALIGNED(struct qnode, qnodes[MAX_NODES])
一个cpu上可以同时运行的spinlock最多4个——task, softirq, hardirq, nmi。所以用数组4表示这个层次。
*/
static DEFINE_PER_CPU_ALIGNED(struct qnode, qnodes[MAX_NODES]); //MAX_NODES=4

/*
 * We must be able to distinguish between no-tail and the tail at 0:0,
 * therefore increment the cpu number by one.
 */

static inline __pure u32 encode_tail(int cpu, int idx)
{
	u32 tail;

	tail  = (cpu + 1) << _Q_TAIL_CPU_OFFSET;
	tail |= idx << _Q_TAIL_IDX_OFFSET; /* assume < 4 */

	return tail;
}

static inline __pure struct mcs_spinlock *decode_tail(u32 tail)
{
	int cpu = (tail >> _Q_TAIL_CPU_OFFSET) - 1;
	int idx = (tail &  _Q_TAIL_IDX_MASK) >> _Q_TAIL_IDX_OFFSET;

	return per_cpu_ptr(&qnodes[idx].mcs, cpu);
}

static inline __pure
struct mcs_spinlock *grab_mcs_node(struct mcs_spinlock *base, int idx)
{
	return &((struct qnode *)base + idx)->mcs;
}

#define _Q_LOCKED_PENDING_MASK (_Q_LOCKED_MASK | _Q_PENDING_MASK)

#if _Q_PENDING_BITS == 8
/**
 * clear_pending - clear the pending bit.
 * @lock: Pointer to queued spinlock structure
 *
 * *,1,* -> *,0,*
 */
static __always_inline void clear_pending(struct qspinlock *lock)
{
	WRITE_ONCE(lock->pending, 0);
}

/**
 * clear_pending_set_locked - take ownership and clear the pending bit.
 * @lock: Pointer to queued spinlock structure
 *
 * *,1,0 -> *,0,1
 *
 * Lock stealing is not allowed if this function is used.
 */
static __always_inline void clear_pending_set_locked(struct qspinlock *lock)
{
	WRITE_ONCE(lock->locked_pending, _Q_LOCKED_VAL);
}

/*
 * xchg_tail - Put in the new queue tail code word & retrieve previous one
 * @lock : Pointer to queued spinlock structure
 * @tail : The new queue tail code word
 * Return: The previous queue tail code word
 *
 * xchg(lock, tail), which heads an address dependency
 *
 * p,*,* -> n,*,* ; prev = xchg(lock, node)
 */
static __always_inline u32 xchg_tail(struct qspinlock *lock, u32 tail)
{
	/*
	 * We can use relaxed semantics since the caller ensures that the
	 * MCS node is properly initialized before updating the tail.
	 */
	return (u32)xchg_relaxed(&lock->tail,
				 tail >> _Q_TAIL_OFFSET) << _Q_TAIL_OFFSET;
}

#else /* _Q_PENDING_BITS == 8 */

/**
 * clear_pending - clear the pending bit.
 * @lock: Pointer to queued spinlock structure
 *
 * *,1,* -> *,0,*
 */
static __always_inline void clear_pending(struct qspinlock *lock)
{
	atomic_andnot(_Q_PENDING_VAL, &lock->val);
}

/**
 * clear_pending_set_locked - take ownership and clear the pending bit.
 * @lock: Pointer to queued spinlock structure
 *
 * *,1,0 -> *,0,1
 */
static __always_inline void clear_pending_set_locked(struct qspinlock *lock)
{
	atomic_add(-_Q_PENDING_VAL + _Q_LOCKED_VAL, &lock->val);
}

/**
 * xchg_tail - Put in the new queue tail code word & retrieve previous one
 * @lock : Pointer to queued spinlock structure
 * @tail : The new queue tail code word
 * Return: The previous queue tail code word
 *
 * xchg(lock, tail)
 *
 * p,*,* -> n,*,* ; prev = xchg(lock, node)
 */
static __always_inline u32 xchg_tail(struct qspinlock *lock, u32 tail)
{
	u32 old, new, val = atomic_read(&lock->val);

	for (;;) {
		new = (val & _Q_LOCKED_PENDING_MASK) | tail;
		/*
		 * We can use relaxed semantics since the caller ensures that
		 * the MCS node is properly initialized before updating the
		 * tail.
		 */
		old = atomic_cmpxchg_relaxed(&lock->val, val, new);
		if (old == val)
			break;

		val = old;
	}
	return old;
}
#endif /* _Q_PENDING_BITS == 8 */

/**
 * queued_fetch_set_pending_acquire - fetch the whole lock value and set pending
 * @lock : Pointer to queued spinlock structure
 * Return: The previous lock value
 *
 * *,*,* -> *,1,*
 */
#ifndef queued_fetch_set_pending_acquire
static __always_inline u32 queued_fetch_set_pending_acquire(struct qspinlock *lock)
{
	return atomic_fetch_or_acquire(_Q_PENDING_VAL, &lock->val);
}
#endif

/**
 * set_locked - Set the lock bit and own the lock
 * @lock: Pointer to queued spinlock structure
 *
 * *,*,0 -> *,0,1
 */
static __always_inline void set_locked(struct qspinlock *lock)
{
	WRITE_ONCE(lock->locked, _Q_LOCKED_VAL);
}


/*
 * Generate the native code for queued_spin_unlock_slowpath(); provide NOPs for
 * all the PV callbacks.
 */

static __always_inline void __pv_init_node(struct mcs_spinlock *node) { }
static __always_inline void __pv_wait_node(struct mcs_spinlock *node,
					   struct mcs_spinlock *prev) { }
static __always_inline void __pv_kick_node(struct qspinlock *lock,
					   struct mcs_spinlock *node) { }
static __always_inline u32  __pv_wait_head_or_lock(struct qspinlock *lock,
						   struct mcs_spinlock *node)
						   { return 0; }

#define pv_enabled()		false

#define pv_init_node		__pv_init_node
#define pv_wait_node		__pv_wait_node
#define pv_kick_node		__pv_kick_node
#define pv_wait_head_or_lock	__pv_wait_head_or_lock

#ifdef CONFIG_PARAVIRT_SPINLOCKS
#define queued_spin_lock_slowpath	native_queued_spin_lock_slowpath
#endif

#endif /* _GEN_PV_LOCK_SLOWPATH */

/**
 * queued_spin_lock_slowpath - acquire the queued spinlock
 * @lock: Pointer to queued spinlock structure
 * @val: Current value of the queued spinlock 32-bit word
 *
 * (queue tail, pending bit, lock value)
 *
 *              fast     :    slow                                  :    unlock
 *                       :                                          :
 * uncontended  (0,0,0) -:--> (0,0,1) ------------------------------:--> (*,*,0)
 *                       :       | ^--------.------.             /  :
 *                       :       v           \      \            |  :
 * pending               :    (0,1,1) +--> (0,1,0)   \           |  :
 *                       :       | ^--'              |           |  :
 *                       :       v                   |           |  :
 * uncontended           :    (n,x,y) +--> (n,0,0) --'           |  :
 *   queue               :       | ^--'                          |  :
 *                       :       v                               |  :
 * contended             :    (*,x,y) +--> (*,0,0) ---> (*,0,1) -'  :
 *   queue               :         ^--'                             :
 */
/*xiaojin-spinlock 3.8-smp(x86)-lock queued_spin_lock_slowpath
参考系列文章：https://app.yinxiang.com/shard/s65/nl/15273355/895ced64-c9d6-4518-863e-639f7466c6e3/ (MCS相关)

*/

/*xiaojin-spinlock-qspinlock -5!!! queued_spin_lock_slowpath
https://app.yinxiang.com/shard/s65/nl/15273355/aca32ad8-d3cd-453e-88af-4af0b5fcfbd8/  (系列3)
https://app.yinxiang.com/shard/s65/nl/15273355/ce2b4805-1bd0-44d8-b97d-6218bdc68db4/  （系列4）

spinlock的上下文一定是关闭中断的，这才使得这个percpu static DEFINE_PER_CPU_ALIGNED(struct qnode, qnodes[MAX_NODES]) 不会超过4层。spinlock是不会让出cpu的，所以适合锁住小而且精悍的代码。要快速返回，又要加锁同步。

对别mcs spinlock的有点是 qsinlock的尺寸小了，只有4个字节。mcs有16个字节。
*/
void queued_spin_lock_slowpath(struct qspinlock *lock, u32 val)
{
	struct mcs_spinlock *prev, *next, *node;
	u32 old, tail;
	int idx;

	BUILD_BUG_ON(CONFIG_NR_CPUS >= (1U << _Q_TAIL_CPU_BITS));

	if (pv_enabled())
		goto pv_queue;

	if (virt_spin_lock(lock))
		return;

	/*
	 * Wait for in-progress pending->locked hand-overs with a bounded
	 * number of spins so that we guarantee forward progress.
	 *
	 * 0,1,0 -> 0,0,1 在这里设置{clear_pending_set_locked(lock);}
	 * 程序解释：如果当前lock->val处于pending为被独自设置的状态，自己肯定是第三了，而且此时第一已经释放锁了，第二碰巧还没设置成（0,0,1），自己应该是第二，所以等真正的第二设置完成我就不要进入队列了，yes！所以我在这里自旋下。
	 */
	if (val == _Q_PENDING_VAL) { //第8位为1 这个状态就是pending位被设置，持有锁的线程已经释放了锁，第二个线程还在自旋等待，还没将其变成（0，0，1），所以val短暂的变成了（0,1,0）。所以等着变回0,0,1）
		int cnt = _Q_PENDING_LOOPS; //第9位为1
		/*像atomic_cond_read_acquire()这种函数，其实就是Linux中的原子操作这篇文章介绍的atomic_read()加上"cond"和"acquire"，其中"cond"代表condition，表示spin基于的对象，而"acquire"用于设置Memory Barrier。这是为了保证应该在获取spinlock之后才能执行的代码，不要因为Memory Order的调换，在成功获取spinlock之前就执行了，那样就失去了保护Critical Section/Region的目的。
		
		等待从0,1,0 -> 0,0,1 最多等512-1次cpu_relax()。如果511次relax还没变成1，则返回。

		val是最近的lock-》val的值。
		*/
		val = atomic_cond_read_relaxed(&lock->val,
					       (VAL != _Q_PENDING_VAL) || !cnt--);//val不等于2^8或者到了spin的次数。其实有可能走到了queue也说不定。
	}

	/*
	 * If we observe any contention; queue.
	 */
	/*
 * Bitfields in the atomic value:
 * 三元组（queue tail, pending bit, lock value）
 * When NR_CPUS < 16K
 *  0- 7: locked byte ----这就是lock value
 *     8: pending  ----- 这就是pending bit
 *  9-15: not used
 * 16-17: tail index
 * 18-31: tail cpu (+1) ---- 这就是queue tail
 *
 * When NR_CPUS >= 16K
 *  0- 7: locked byte
 *     8: pending
 *  9-10: tail index
 * 11-31: tail cpu (+1)
 */

/* 
_Q_LOCKED_MASK 0000 0000 1111 1111 8个1
~_Q_LOCKED_MASK = 1111 1111 0000 0000
如果是真，说明pending或者tail cpu位为1。

程序解释：说明前一步等到（0，0,1）或者步数过了以后，pending或者tail cpu有值。进入if，说明自己没有当成第二，肯定不是前三了，要去排队了。好伤心！
*/
	if (val & ~_Q_LOCKED_MASK) //接上面，果然到了queue这个阶段了，不止2个CPU抢到了锁，我其实应该去Percpu的本地锁自旋了。
		goto queue;

	/*
	将自己当做第二个CPU（当然可能不是），然后设置lock->val的pending位。

	 * trylock || pending
	 *
	 * 0,0,* -> 0,1,* -> 0,0,1 pending, trylock
	 * 如果如愿以偿的到达（0,0,1）说明自己是第二个竞争的人，不用说，将pending位设置成1，val变成(0,1,1) 。
	 * 
	 * 实际调用：atomic_fetch_or_acquire(_Q_PENDING_VAL, &lock->val)
	 * 最后调用到：
	 * 
	 * static __always_inline int arch_atomic_fetch_or(int i, atomic_t *v)
		{
			int val = arch_atomic_read(v); //READ_ONCE
			do { } while (!arch_atomic_try_cmpxchg(v, &val, val | i)); //lock->val就是v，i就是_Q_PENDING_VAL
			return val;
		}

		程序解释：也就是，如果v==val 则将val=v（返回v更新前的值）；v=val|i；返回true；
		如果v!=val 则val=v；返回false

		程序解释：到这里来了，说明自己很幸运，自己可能抢到了第二把交椅。但是，也不能高兴太早，可能在执行上面的if时，其他CPU把第二把交椅抢了。但是，不管怎么样，先原子的设置pending位。涉及到cmpxchg指令，val的值是设置pending位之前的lock->val值！
	 */
	val = queued_fetch_set_pending_acquire(lock);//这里一定是设置val|之前的lock-》val值。可能因为竞争被其他cpu抢先设置了pending为，也可能正常的不带pending位！！！！

	/*
	 * If we observe contention, there is a concurrent locker.
	 *
	 * Undo and queue; our setting of PENDING might have made the
	 * n,0,0 -> 0,0,0 transition fail and it will now be waiting
	 * on @next to become !NULL.
	 */

	/*
	程序解释：补丁程序，因为是无锁编程，所以自己在设置完pending位后可能不是第二个CPU了，要去排队了…… 如果进入if，太遗憾了，在第二次竞争中没有获胜（tail-cpu或者pending被其他CPU占有了），可能还需要撤掉多设置的pending。
	*/
	if (unlikely(val & ~_Q_LOCKED_MASK)) {// 正常情况，val应该是（0,0,1）所以分支是进不来的，而且大部分情况如此（没有激烈的contention）所以unlikely暗示CPU不要预测true；但是在contention的情况，lock可能已经超过两个cpu在等待了，此时依然可以设置pending位成功，所以针对这种情况需要做当前的判断。如果真的发生contention，则需要queue。

		/* Undo PENDING if we set it. */
		if (!(val & _Q_PENDING_MASK)) //因为进入了if，所以自己肯定不是第二了，要进入队列，此时应该不要设置lock的pending位，老实去排队。但是，之前连消带打已经设置了，所以如果设置之前的lock->pending也就是val的pending位已经被设置，说明此前只是重复设置了，没事。但是如果之前没有设置pending，自己应该直接去排队，这里要回滚！
			clear_pending(lock);

		goto queue;
	}

	/*
	 * We're pending, wait for the owner to go away.
	 *
	 * 0,1,1 -> 0,1,0
	 *
	 * this wait loop must be a load-acquire such that we match the
	 * store-release that clears the locked bit and create lock
	 * sequentiality; this is because not all
	 * clear_pending_set_locked() implementations imply full
	 * barriers.
	 */
	/*
		#define smp_cond_load_relaxed(ptr, cond_expr) ({		\
			typeof(ptr) __PTR = (ptr);				\
			__unqual_scalar_typeof(*ptr) VAL;			\
			for (;;) {						\
				VAL = READ_ONCE(*__PTR);			\
				if (cond_expr)					\
					break;					\
				cpu_relax();					\
			}							\
			(typeof(*ptr))VAL;					\
		})

	程序解释：（走到这里val是(0,0,1）lock->val是(0,1,1)),太幸运了，自己就是第二名，前面的CPU占有了锁，它执行完就是我了！开心！在这里自旋下吧，马上了。等着VAL & _Q_LOCKED_MASK变成0.也就是（0,0,0）

	SPIN-1:第一个等待的CPU在这里自旋！！！等待从(0,1,1)到（0,1,0）因为spin_unlock()只会将lock->val的lock字段清除，所以只能变成(0,1,0)。
	此时val正常应该是（0,0,1）这其实是说自己在等待前面获得锁的CPU释放锁。此时lock->val == (0,1,1)
	那么就在这里自旋，谁叫自己是第一个等待的CPU呢？自旋等待lock->变成（0,1,0）也就是等待持有锁的CPU释放锁。条件是(VAL & _Q_LOCKED_MASK) */
	if (val & _Q_LOCKED_MASK)
		atomic_cond_read_acquire(&lock->val, !(VAL & _Q_LOCKED_MASK));

	/*
	 * take ownership and clear the pending bit.
	 *
	 * 0,1,0 -> 0,0,1
	 */
	/*程序解释：好了，我是老大了，设置成（0,0,1）我抢到锁了，退出去干正事了~~开心！
	
	SPIN-1.1 这里就呼应了开头{if (val == _Q_PENDING_VAL) 这个分支}*/
	clear_pending_set_locked(lock); //从（0,1,0）变成（0,0,1） 这就是为什么开头判断val==_Q_PENDING_VAL的含义。等的就是这个操作。
	lockevent_inc(lock_pending);
	return;

	/*
	 * End of pending bit optimistic spinning and beginning of MCS
	 * queuing.
	 */

	/*程序解释：下面都是大于1个等待线程的处理情况，排队把……没办法。
	自己可能是第三或者很后面，这里要分开对待。
	lock->val的值tail_cpu或者pending不同时是0.
	*/
queue:
	lockevent_inc(lock_slowpath);
pv_queue:
	node = this_cpu_ptr(&qnodes[0].mcs);//第0个node
	idx = node->count++;//嵌套层数放在了第一个元素的count中。
	tail = encode_tail(smp_processor_id(), idx);//设置tail_cpu的值， 9-10: tail index 11-31: tail cpu (+1)——加1是为了避免0表示没有CPU设置，但是有表示第0号CPU，所以有矛盾，干脆就+1.

	/*
	 * 4 nodes are allocated based on the assumption that there will
	 * not be nested NMIs taking spinlocks. That may not be true in
	 * some architectures even though the chance of needing more than
	 * 4 nodes will still be extremely unlikely. When that happens,
	 * we fall back to spinning on the lock directly without using
	 * any MCS node. This is not the most elegant solution, but is
	 * simple enough.
	 */
	if (unlikely(idx >= MAX_NODES)) {
		lockevent_inc(lock_no_node);
		while (!queued_spin_trylock(lock))
			cpu_relax();
		goto release;
	}

	/*程序解释：node是本CPU的空mcs节点，应该是空的没用过的。*/
	node = grab_mcs_node(node, idx);//node初始化为qnodes[0].mcs percpu数组的第0个mcs。一共有4个，表示每个CPU上不管多少个qspinlock对象最多也只能嵌套4层（task si hi nmi）所以每个数组成员表示一层。这里就是根据idex取出相应层的mcs对象。

	/*
	 * Keep counts of non-zero index values:
	 */
	lockevent_cond_inc(lock_use_node2 + idx - 1, idx);

	/*
	 * Ensure that we increment the head node->count before initialising
	 * the actual node. If the compiler is kind enough to reorder these
	 * stores, then an IRQ could overwrite our assignments.
	 */
	barrier(); //就是防止编译器reorder后面对node的操作必须要在得到正确的node之后才能进行，否则会乱套。因为intel是强一致性内存模型。intel内存模型，memery-reorder 参考：https://app.yinxiang.com/shard/s65/nl/15273355/b9569249-ef01-4141-927f-8ae713b8770a/

	/*初始化node节点。
	*/
	node->locked = 0; //mcs spinlock的locked字段为1的时候说明占有了锁。
	node->next = NULL;//因为自己应该是tail所以没有next。
	pv_init_node(node);

	/*
	 * We touched a (possibly) cold cacheline in the per-cpu queue node;
	 * attempt the trylock once more in the hope someone let go while we
	 * weren't watching.
	 */

	/*
	static __always_inline int queued_spin_trylock(struct qspinlock *lock)
	{
		int val = atomic_read(&lock->val);

		if (unlikely(val))
			return 0;

		return likely(atomic_try_cmpxchg_acquire(&lock->val, &val, _Q_LOCKED_VAL));
	}

	程序解释：嗷嗷嗷！！！如果此时lock->val==0，（进入了if）也就是都释放干净了，太有趣了吧！！！以为自己要排队，结果转了一圈发现就剩自己了！！！！自己要从发配边疆马上要到继承大统，这过山车坐得。这个指令，我连皇冠都带上了！
	已经（0,0,1）了。直接去release。将嵌套counter回收下就去当皇帝了。
	*/
	if (queued_spin_trylock(lock))
		goto release;

	/****************************下面正式走queue的流程。/
	/*
	 * Ensure that the initialisation of @node is complete before we
	 * publish the updated tail via xchg_tail() and potentially link
	 * @node into the waitqueue via WRITE_ONCE(prev->next, node) below.
	 */
	
	/*
	#define wmb()	asm volatile("sfence" ::: "memory")
	store fence后面的指令不能提前到这个之前
	保证没有做完判断、取到正确的node不做更新。
	*/
	smp_wmb();

	/*
	 * Publish the updated tail.
	 * We have already touched the queueing cacheline; don't bother with
	 * pending stuff.
	 *
	 * p,*,* -> n,*,*
	 */

	/*程序解释：好了，正常流程，不幸要开始排队了！xchg_tail连消带打，一条指令将lock->tail_cpu设置成自己，自己先排到最后，然后保存前面的tail_cpu值到old中。*/
	old = xchg_tail(lock, tail);
	next = NULL;

	/*
	 * if there was a previous node; link it and wait until reaching the
	 * head of the waitqueue.
	 */

	/*程序解释：判断自己是否为queue的第一个元素,if退出以后node就是了，要在这里做自旋等待msc->locked变成1*/
	if (old & _Q_TAIL_MASK) {//如果不是，这是最坏的情况了…………可能要等很久，伤心……

		/*找到自己的前驱节点。*/
		prev = decode_tail(old); //获取old对应的cpu的percpu qnodes队列，并取出之前最后一个msc对象。因为idx与cpu都编码到了old变量里面所以可以轻松拿到。

		/* Link @node into the waitqueue. */
		/*是不是很简单，尾插法嘛*/
		WRITE_ONCE(prev->next, node); //前驱的next原来是NULL。

		pv_wait_node(node, prev);

		/*
		smp_cond_load_acquire(l, VAL);	
		|
		v
		#define smp_cond_load_relaxed(ptr, cond_expr) ({		\
			typeof(ptr) __PTR = (ptr);				\
			__unqual_scalar_typeof(*ptr) VAL;			\
			for (;;) {						\
				VAL = READ_ONCE(*__PTR);			\
				if (cond_expr)---VAL					\
					break;					\
				cpu_relax();					\
			}							\
		(typeof(*ptr))VAL;					\
		})

		SPIN-2 在自己CPU上自旋。等待自己变成队头。node->locked==1的情况发生。
		程序解释：好了开始等待通知了，击鼓传花的方式，我看看花啥时候到我手里了。在这自旋把。
		如果msc->locked==1,也就是VAL==1，说明轮到自己执行了，自己获得了锁，所以cpu_relax()结束。否则自旋。
		说白了就是等待自己的percpu msc被别的CPU因为释放锁而设置成1.

		执行完以后node就是队列头了！
		
		在哪里设置这个node->locked=1呢？在函数的最后。arch_mcs_spin_unlock_contended(&next->locked);
		这里有个地方要注意：
		在前一个CPU获得qspinlock锁以后，也就是lock->val.locked=1的时候，已经指定了下一个mcs node的locked=1了。这里要注意。是先设置“下一个”mcs抢到了lock，然后再设置本CPU的qspinlock.val.locked为1.这样，当前CPU可以去执行业务逻辑，而不用担心下一个CPU获得锁。因为，next会在SPIN-3的地方等待你解锁。也就是将qspinlock.val.locked=0设置成0.
		*/
		arch_mcs_spin_lock_contended(&node->locked);

		/*
		 * While waiting for the MCS lock, the next pointer may have
		 * been set by another lock waiter. We optimistically load
		 * the next pointer & prefetch the cacheline for writing
		 * to reduce latency in the upcoming MCS unlock operation.
		 */
		/*下一个节点缓存预热下*/
		next = READ_ONCE(node->next);
		if (next)
			prefetchw(next);
	}

	/*
	 * we're at the head of the waitqueue, wait for the owner & pending to
	 * go away.
	 *
	 * *,x,y -> *,0,0
	 *
	 * this wait loop must use a load-acquire such that we match the
	 * store-release that clears the locked bit and create lock
	 * sequentiality; this is because the set_locked() function below
	 * does not imply a full barrier.
	 *
	 * The PV pv_wait_head_or_lock function, if active, will acquire
	 * the lock and return a non-zero value. So we have to skip the
	 * atomic_cond_read_acquire() call. As the next PV queue head hasn't
	 * been designated yet, there is no way for the locked value to become
	 * _Q_SLOW_VAL. So both the set_locked() and the
	 * atomic_cmpxchg_relaxed() calls will be safe.
	 *
	 * If PV isn't active, 0 will be returned instead.
	 *
	 */
	if ((val = pv_wait_head_or_lock(lock, node)))
		goto locked;

	/*程序解释：走到这里说明现在我-node是队头了！！！----------------》也就是自己就是第一个queue，最多第3个cpu。可能是(n,0,0)或者(n,*,1)*/

	/*
	_Q_LOCKED_PENDING_MASK == 0000 0001 1111 1111
	有两种情况到这里：
	1、old==0 也就是lock->val的tail_cpu为空。说明一开始node就是队列头，说明此时有2个CPU在等锁（包括自己），一个占有锁；
	2、在上一步自旋结果中，node的msc->locked被设置成1，此时已经轮到自己占有锁了。

	展开：
		#define smp_cond_load_relaxed(ptr, cond_expr) ({		\
		typeof(ptr) __PTR = (ptr);				\
		__unqual_scalar_typeof(*ptr) VAL;			\
		for (;;) {						\
			VAL = READ_ONCE(*__PTR);			\
			if (cond_expr)					\
				break;					\
			cpu_relax();					\
		}							\
		(typeof(*ptr))VAL;					\
		})

	两种情况都在这里自旋。SPIN-3 也就是等待(n,*,1)到（n,0,0）自己已经是队列头了，但是也得等qspinlock的前两个CPU释放锁!
	等待前面两个CPU释放锁（VAL & _Q_LOCKED_PENDING_MASK)也就是lock->val前两位变成0），因为前面已经证明到这里说明自己是队头，执行这条spin后退出说明前面的pending与lock都是0了，说明终于轮到自己了，就结束自旋，等待加冕了~
	*/
	val = atomic_cond_read_acquire(&lock->val, !(VAL & _Q_LOCKED_PENDING_MASK)); //_Q_LOCKED_PENDING_MASK lock->val前两个字节全部是1。如果lock与pending都没有了，也就是第1第2都释放了，就轮到queue的第一个了。也就是我！val是最新拿到的lock-》val。

locked:
	/*
	 * claim the lock:
	 *
	 * n,0,0 -> 0,0,1 : lock, uncontended
	 * *,*,0 -> *,*,1 : lock, contended
	 *
	 * If the queue head is the only one in the queue (lock value == tail)
	 * and nobody is pending, clear the tail code and grab the lock.
	 * Otherwise, we only need to grab the lock.
	 */

	/*
	 * In the PV case we might already have _Q_LOCKED_VAL set, because
	 * of lock stealing; therefore we must also allow:
	 *
	 * n,0,1 -> 0,0,1
	 *
	 * Note: at this point: (val & _Q_PENDING_MASK) == 0, because of the
	 *       above wait condition, therefore any concurrent setting of
	 *       PENDING will make the uncontended transition fail.
	 */

	/*
	程序说明：自己是队头也是队尾吗？如果是，很幸运，你是最后一个了，直接设置成(0,0,1)然后去愉快的运行把~
	*/
	if ((val & _Q_TAIL_MASK) == tail) {//自己就是tail也是head
		if (atomic_try_cmpxchg_relaxed(&lock->val, &val, _Q_LOCKED_VAL))
			goto release; /* No contention */
	}

	/*
	 * Either somebody is queued behind us or _Q_PENDING_VAL got set
	 * which will then detect the remaining tail and queue behind us
	 * ensuring we'll see a @next.
	 */
	/*程序解释：走到这里说明后面还有tail，自己不是最后一个。所以设置lock->locked=1，说明我拥有锁*/
	set_locked(lock);

	/*
	 * contended path; wait for next if not observed yet, release.
	 */

	/*
	程序说明：因为要加入一个新的node的过程是：
	1、old = xchg_tail(lock, tail); 修改lock的tail_cpu字段到当前cpu；
	2、WRITE_ONCE(prev->next, node);然后再修改的next指针
	3、因为前面已经判断自己是否是队尾，所以肯定是执行了1、还没到2、。
	所以，next还是null不能说明没有下一个node，可能还没到。所以自旋等待一下，等next被设置。
	*/
	if (!next)
		next = smp_cond_load_relaxed(&node->next, (VAL));

	/*我走完最后历程从(0,0,1)->(n,*,1)->queued->wait msc->locked to 1->now set next.locked=1
	我要加冕了。
	最后一步指定继任者，然后设置它的mcs->locked=1，它可以准备跑了。

	smp_store_release((l), 1) 在X86-64下等价于WRITE_ONCE(prev->next, node);
	将next->locked设置成1.
	*/
	arch_mcs_spin_unlock_contended(&next->locked);
	pv_kick_node(lock, next);

release:
	/*
	 * release the node
	 */

	/*退出锁，嵌套减回去*/
	__this_cpu_dec(qnodes[0].mcs.count);
}
EXPORT_SYMBOL(queued_spin_lock_slowpath);

/*
 * Generate the paravirt code for queued_spin_unlock_slowpath().
 */
#if !defined(_GEN_PV_LOCK_SLOWPATH) && defined(CONFIG_PARAVIRT_SPINLOCKS)
#define _GEN_PV_LOCK_SLOWPATH

#undef  pv_enabled
#define pv_enabled()	true

#undef pv_init_node
#undef pv_wait_node
#undef pv_kick_node
#undef pv_wait_head_or_lock

#undef  queued_spin_lock_slowpath
#define queued_spin_lock_slowpath	__pv_queued_spin_lock_slowpath

#include "qspinlock_paravirt.h"
#include "qspinlock.c"

bool nopvspin __initdata;
static __init int parse_nopvspin(char *arg)
{
	nopvspin = true;
	return 0;
}
early_param("nopvspin", parse_nopvspin);
#endif
