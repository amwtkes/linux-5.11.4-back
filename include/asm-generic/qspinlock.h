/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Queued spinlock
 *
 * (C) Copyright 2013-2015 Hewlett-Packard Development Company, L.P.
 * (C) Copyright 2015 Hewlett-Packard Enterprise Development LP
 *
 * Authors: Waiman Long <waiman.long@hpe.com>
 */
#ifndef __ASM_GENERIC_QSPINLOCK_H
#define __ASM_GENERIC_QSPINLOCK_H

#include <asm-generic/qspinlock_types.h>
#include <linux/atomic.h>

#ifndef queued_spin_is_locked
/**
 * queued_spin_is_locked - is the spinlock locked?
 * @lock: Pointer to queued spinlock structure
 * Return: 1 if it is locked, 0 otherwise
 */
static __always_inline int queued_spin_is_locked(struct qspinlock *lock)
{
	/*
	 * Any !0 state indicates it is locked, even if _Q_LOCKED_VAL
	 * isn't immediately observable.
	 */
	return atomic_read(&lock->val);
}
#endif

/**
 * queued_spin_value_unlocked - is the spinlock structure unlocked?
 * @lock: queued spinlock structure
 * Return: 1 if it is unlocked, 0 otherwise
 *
 * N.B. Whenever there are tasks waiting for the lock, it is considered
 *      locked wrt the lockref code to avoid lock stealing by the lockref
 *      code and change things underneath the lock. This also allows some
 *      optimizations to be applied without conflict with lockref.
 */
static __always_inline int queued_spin_value_unlocked(struct qspinlock lock)
{
	return !atomic_read(&lock.val);
}

/**
 * queued_spin_is_contended - check if the lock is contended
 * @lock : Pointer to queued spinlock structure
 * Return: 1 if lock contended, 0 otherwise
 */
static __always_inline int queued_spin_is_contended(struct qspinlock *lock)
{
	return atomic_read(&lock->val) & ~_Q_LOCKED_MASK;
}
/**
 * queued_spin_trylock - try to acquire the queued spinlock
 * @lock : Pointer to queued spinlock structure
 * Return: 1 if lock acquired, 0 if failed
 */
static __always_inline int queued_spin_trylock(struct qspinlock *lock)
{
	int val = atomic_read(&lock->val);

	if (unlikely(val))
		return 0;

	return likely(atomic_try_cmpxchg_acquire(&lock->val, &val, _Q_LOCKED_VAL));
}

extern void queued_spin_lock_slowpath(struct qspinlock *lock, u32 val);

#ifndef queued_spin_lock
/**
 * queued_spin_lock - acquire a queued spinlock
 * @lock: Pointer to queued spinlock structure
 */
/*xiaojin-spinlock 3.6-smp(x86)-lock  queued_spin_lock
*/
/*xiaojin-spinlock-qspinlock -4 queued_spin_lock*/
static __always_inline void queued_spin_lock(struct qspinlock *lock)
{
	int val = 0;
	//原子指令来比较是否获得了锁。如果lock->val是0(val)，则说明获取了锁，把lock->val变成_Q_LOCKED_VAL==1。参考3.7
	//如果lock->val == val==0 则lock->val=1,返回val==0；获得锁；
	//如果lock->val != val，则val=lock->val，返回lock->val

	/*
	xiaojin-spinlock 3.6.1-smp(x86)-lock atomic_try_cmpxchg_acquire是一个原子操作，如果第一个参数指向的原子类型变量的值和第二个参数指向的变量的值相同，则将第一个参数指向的原子变量赋值成第三个参数的值，并返回真；否则，则读取第一个参数指向的原子变量的值，将其赋值给第二个参数指向的变量，并返回假。
	*/
	if (likely(atomic_try_cmpxchg_acquire(&lock->val, &val, _Q_LOCKED_VAL)))
		return;

	queued_spin_lock_slowpath(lock, val);
}
#endif

#ifndef queued_spin_unlock
/**
 * queued_spin_unlock - release a queued spinlock
 * @lock : Pointer to queued spinlock structure
 */
/*xiaojin-spinlock-qspinlock -6 queued_spin_unlock*/
static __always_inline void queued_spin_unlock(struct qspinlock *lock)
{
	/*
	 * unlock() needs release semantics:
	 */
	//注意知识把locled变成了0，可能pending还是1.
	smp_store_release(&lock->locked, 0);
}
#endif

#ifndef virt_spin_lock
static __always_inline bool virt_spin_lock(struct qspinlock *lock)
{
	return false;
}
#endif

/*
 * Remapping spinlock architecture specific functions to the corresponding
 * queued spinlock functions.
 */
#define arch_spin_is_locked(l)		queued_spin_is_locked(l)
#define arch_spin_is_contended(l)	queued_spin_is_contended(l)
#define arch_spin_value_unlocked(l)	queued_spin_value_unlocked(l)
/*xiaojin-spinlock 3.5-smp(x86)-lock  define arch_spin_lock(l)		queued_spin_lock(l)*/
#define arch_spin_lock(l)		queued_spin_lock(l)
#define arch_spin_trylock(l)		queued_spin_trylock(l)
#define arch_spin_unlock(l)		queued_spin_unlock(l)

#endif /* __ASM_GENERIC_QSPINLOCK_H */
