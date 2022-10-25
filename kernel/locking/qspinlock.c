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

/*xiaojin-spinlock-qspinlock -5 queued_spin_lock
https://app.yinxiang.com/shard/s65/nl/15273355/aca32ad8-d3cd-453e-88af-4af0b5fcfbd8/  (系列3)
https://app.yinxiang.com/shard/s65/nl/15273355/ce2b4805-1bd0-44d8-b97d-6218bdc68db4/  （系列4）

spinlock的上下文一定是关闭中断的，这才使得这个percpu static DEFINE_PER_CPU_ALIGNED(struct qnode, qnodes[MAX_NODES]) 不会超过4层。spinlock是不会让出cpu的，所以适合锁住小而且精悍的代码。要快速返回，又要加锁同步。
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

说明前一步等到（0，0,1）或者步数过了以后，pending或者tail cpu有值。说明还是有竞争。因为如果等到了（0,0,1）现在又不是了，或者甚至已经到tail了。又或者，等到超过自旋次数了还是在pending，还没有回到（0，0,1）不过当loop设定成512次以后前一种的情况可能性更高，也就是出现了激烈的竞争。
没办法进入队列处理。
*/
	if (val & ~_Q_LOCKED_MASK) //接上面，果然到了queue这个阶段了，不止2个CPU抢到了锁，我其实应该去Percpu的本地锁自旋了。
		goto queue;

	/*
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

		也就是，如果v==val 则将val=v（返回v更新前的值）；v=val|i；返回true；
		如果v!=val 则val=v；返回false
	 */
	val = queued_fetch_set_pending_acquire(lock);//这里一定是设置val|之前的lock-》val值。可能因为竞争被其他cpu抢先设置了pending为，也可能正常的不带pending位！！！！

	/*
	 * If we observe contention, there is a concurrent locker.
	 *
	 * Undo and queue; our setting of PENDING might have made the
	 * n,0,0 -> 0,0,0 transition fail and it will now be waiting
	 * on @next to become !NULL.
	 */
	if (unlikely(val & ~_Q_LOCKED_MASK)) {// 正常情况，val应该是（0,0,1）所以分支是进不来的，而且大部分情况如此（没有激烈的contention）所以unlikely暗示CPU不要预测true；但是在contention的情况，lock可能已经超过两个cpu在等待了，此时依然可以设置pending位成功，所以针对这种情况需要做当前的判断。如果真的发生contention，则需要queue。

		/* Undo PENDING if we set it. */
		if (!(val & _Q_PENDING_MASK)) //val是抢完之前的值，如果没有设置pending，也就是此时我已经错误的将lock-》val已经设置了，所以要退回去，恢复之前的状态。也就是执行queued_fetch_set_pending_acquire完成之前，qspinlock变量已经进入tail_cpu阶段了，且（cpu,0,0）的状态了。所以要回滚！！！！
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
	/*第一个等待的CPU在这里自旋！！！
	此时val正常应该是（0,0,1）这其实是说自己在等待前面获得锁的CPU释放锁。此时lock->val == (0,1,1)
	那么就在这里自旋，谁叫自己是第一个等待的CPU呢？自旋等待lock->变成（0,1,0）也就是等待持有锁的CPU释放锁。条件是(VAL & _Q_LOCKED_MASK) */
	if (val & _Q_LOCKED_MASK)
		atomic_cond_read_acquire(&lock->val, !(VAL & _Q_LOCKED_MASK));

	/*
	 * take ownership and clear the pending bit.
	 *
	 * 0,1,0 -> 0,0,1
	 */
	/*这里就呼应了开头{if (val == _Q_PENDING_VAL) 这个分支}*/
	clear_pending_set_locked(lock);
	lockevent_inc(lock_pending);
	return;

	/*
	 * End of pending bit optimistic spinning and beginning of MCS
	 * queuing.
	 */

	/*下面都是大于1个等待线程的处理情况*/

queue:
	lockevent_inc(lock_slowpath);
pv_queue:
	node = this_cpu_ptr(&qnodes[0].mcs);//第0个node
	idx = node->count++;
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

	/*node是本CPU的空mcs节点，应该是空的没用过的。*/
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
	如果此时lock->val==0也就是第一第二CPU都释放了，毕竟此时已经很久没看看lock的值了。都释放了，那么自己可以尝试去抢得qspinlock锁，通过cmpxchg指令抢到，并返回true，此时lock->val的值也被设置成了（0,0,1）。可以进入release流程了。
	否则，也就是lock->val不是0，至少自己还要等，则return 0出来了。不能走release。

	所以这个过程就是在进行queue之前再看看是否要走queue的流程。毕竟CPU很快嘛。
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

	/*lock->tail_cpu就是指向当前最后一个cpu node；现在我们要更新lock->tail字段，使用xchg_tail方法。
	更新lock->tail=tail，返回old就是更新之前的值。应该指向之前最后一个cpu的mcs。*/
	old = xchg_tail(lock, tail);
	next = NULL;

	/*
	 * if there was a previous node; link it and wait until reaching the
	 * head of the waitqueue.
	 */

	/*判断自己是否为queue的第一个元素*/
	if (old & _Q_TAIL_MASK) {//如果不是

		/*找到自己的前驱节点。*/
		prev = decode_tail(old); //获取old对应的cpu的percpu qnodes队列，并取出之前最后一个msc对象。因为idx与cpu都编码到了old变量里面所以可以轻松拿到。

		/* Link @node into the waitqueue. */
		/*是不是很简单，尾插法嘛*/
		WRITE_ONCE(prev->next, node); //前驱的next原来是NULL。

		pv_wait_node(node, prev);
		arch_mcs_spin_lock_contended(&node->locked);

		/*
		 * While waiting for the MCS lock, the next pointer may have
		 * been set by another lock waiter. We optimistically load
		 * the next pointer & prefetch the cacheline for writing
		 * to reduce latency in the upcoming MCS unlock operation.
		 */
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

	/*如果是queue中的头节点----------------》old是0 也就是自己就是第一个queue即第3个cpu。*/

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
	if ((val & _Q_TAIL_MASK) == tail) {//自己就是tail也是head
		if (atomic_try_cmpxchg_relaxed(&lock->val, &val, _Q_LOCKED_VAL))
			goto release; /* No contention */
	}

	/*
	 * Either somebody is queued behind us or _Q_PENDING_VAL got set
	 * which will then detect the remaining tail and queue behind us
	 * ensuring we'll see a @next.
	 */
	set_locked(lock);

	/*
	 * contended path; wait for next if not observed yet, release.
	 */
	if (!next)
		next = smp_cond_load_relaxed(&node->next, (VAL));

	arch_mcs_spin_unlock_contended(&next->locked);
	pv_kick_node(lock, next);

release:
	/*
	 * release the node
	 */
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
