#ifndef __LINUX_SPINLOCK_API_UP_H
#define __LINUX_SPINLOCK_API_UP_H

#ifndef __LINUX_SPINLOCK_H
# error "please don't include this file directly"
#endif

/*
 * include/linux/spinlock_api_up.h
 *
 * spinlock API implementation on UP-nondebug (inlined implementation)
 *
 * portions Copyright 2005, Red Hat, Inc., Ingo Molnar
 * Released under the General Public License (GPL).
 */

#define in_lock_functions(ADDR)		0

#define assert_raw_spin_locked(lock)	do { (void)(lock); } while (0)

/*
 * In the UP-nondebug case there's no real locking going on, so the
 * only thing we have to do is to keep the preempt counts and irq
 * flags straight, to suppress compiler warnings of unused lock
 * variables, and to add the proper checker annotations:
 */

/*xiaojin-spinlock 3.2-up-lock ___LOCK(lock) 根据上面的注释可以看到，则up系统下不需要实际的lock过程，为了防止编译器报警（定义了lock但是没有操作）所以用___LOCK(lock)来消除编译器报警。这里只是preempt_disable() 禁止了进程切换就能达到。*/
#define ___LOCK(lock) \
  do { __acquire(lock); (void)(lock); } while (0)

#define __LOCK(lock) \
  do { preempt_disable(); ___LOCK(lock); } while (0)

#define __LOCK_BH(lock) \
  do { __local_bh_disable_ip(_THIS_IP_, SOFTIRQ_LOCK_OFFSET); ___LOCK(lock); } while (0)

#define __LOCK_IRQ(lock) \
  do { local_irq_disable(); __LOCK(lock); } while (0)

#define __LOCK_IRQSAVE(lock, flags) \
  do { local_irq_save(flags); __LOCK(lock); } while (0)

#define ___UNLOCK(lock) \
  do { __release(lock); (void)(lock); } while (0)

#define __UNLOCK(lock) \
  do { preempt_enable(); ___UNLOCK(lock); } while (0)

#define __UNLOCK_BH(lock) \
  do { __local_bh_enable_ip(_THIS_IP_, SOFTIRQ_LOCK_OFFSET); \
       ___UNLOCK(lock); } while (0)

#define __UNLOCK_IRQ(lock) \
  do { local_irq_enable(); __UNLOCK(lock); } while (0)

#define __UNLOCK_IRQRESTORE(lock, flags) \
  do { local_irq_restore(flags); __UNLOCK(lock); } while (0)

/*xiaojin-spinlock 3.1-up-lock -define _raw_spin_lock(lock)			__LOCK(lock)*/
#define _raw_spin_lock(lock)			__LOCK(lock)
#define _raw_spin_lock_nested(lock, subclass)	__LOCK(lock)
#define _raw_read_lock(lock)			__LOCK(lock)
#define _raw_write_lock(lock)			__LOCK(lock)
#define _raw_spin_lock_bh(lock)			__LOCK_BH(lock)
#define _raw_read_lock_bh(lock)			__LOCK_BH(lock)
#define _raw_write_lock_bh(lock)		__LOCK_BH(lock)
#define _raw_spin_lock_irq(lock)		__LOCK_IRQ(lock)
#define _raw_read_lock_irq(lock)		__LOCK_IRQ(lock)
#define _raw_write_lock_irq(lock)		__LOCK_IRQ(lock)
#define _raw_spin_lock_irqsave(lock, flags)	__LOCK_IRQSAVE(lock, flags)
#define _raw_read_lock_irqsave(lock, flags)	__LOCK_IRQSAVE(lock, flags)
#define _raw_write_lock_irqsave(lock, flags)	__LOCK_IRQSAVE(lock, flags)
#define _raw_spin_trylock(lock)			({ __LOCK(lock); 1; })
#define _raw_read_trylock(lock)			({ __LOCK(lock); 1; })
#define _raw_write_trylock(lock)			({ __LOCK(lock); 1; })
#define _raw_spin_trylock_bh(lock)		({ __LOCK_BH(lock); 1; })
#define _raw_spin_unlock(lock)			__UNLOCK(lock)
#define _raw_read_unlock(lock)			__UNLOCK(lock)
#define _raw_write_unlock(lock)			__UNLOCK(lock)
#define _raw_spin_unlock_bh(lock)		__UNLOCK_BH(lock)
#define _raw_write_unlock_bh(lock)		__UNLOCK_BH(lock)
#define _raw_read_unlock_bh(lock)		__UNLOCK_BH(lock)
#define _raw_spin_unlock_irq(lock)		__UNLOCK_IRQ(lock)
#define _raw_read_unlock_irq(lock)		__UNLOCK_IRQ(lock)
#define _raw_write_unlock_irq(lock)		__UNLOCK_IRQ(lock)
#define _raw_spin_unlock_irqrestore(lock, flags) \
					__UNLOCK_IRQRESTORE(lock, flags)
#define _raw_read_unlock_irqrestore(lock, flags) \
					__UNLOCK_IRQRESTORE(lock, flags)
#define _raw_write_unlock_irqrestore(lock, flags) \
					__UNLOCK_IRQRESTORE(lock, flags)

#endif /* __LINUX_SPINLOCK_API_UP_H */
