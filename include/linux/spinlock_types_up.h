#ifndef __LINUX_SPINLOCK_TYPES_UP_H
#define __LINUX_SPINLOCK_TYPES_UP_H

#ifndef __LINUX_SPINLOCK_TYPES_H
# error "please don't include this file directly"
#endif

/*
 * include/linux/spinlock_types_up.h - spinlock type definitions for UP
 *
 * portions Copyright 2005, Red Hat, Inc., Ingo Molnar
 * Released under the General Public License (GPL).
 */

#ifdef CONFIG_DEBUG_SPINLOCK

typedef struct {
	volatile unsigned int slock;
} arch_spinlock_t;

#define __ARCH_SPIN_LOCK_UNLOCKED { 1 }

#else
/*xiaojin-spinlock 2.1 up系统的定义是空，什么都没有，一切都是空啊。当然，这也符合前面的分析，对于UP，即便是打开的preempt选项，所谓的spin lock也不过就是disable preempt而已，不需定义什么spin lock的变量。
禁止中断就相当于这段代码可以一直由这个进程执行完，虽然并发差，但是只有一个CPU不会有太大影响。
*/
typedef struct { } arch_spinlock_t;

#define __ARCH_SPIN_LOCK_UNLOCKED { }

#endif

typedef struct {
	/* no debug version on UP */
} arch_rwlock_t;

#define __ARCH_RW_LOCK_UNLOCKED { }

#endif /* __LINUX_SPINLOCK_TYPES_UP_H */
