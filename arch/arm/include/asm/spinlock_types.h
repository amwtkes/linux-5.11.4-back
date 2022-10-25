/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_SPINLOCK_TYPES_H
#define __ASM_SPINLOCK_TYPES_H

#ifndef __LINUX_SPINLOCK_TYPES_H
# error "please don't include this file directly"
#endif

#define TICKET_SHIFT	16

/*xiaojin-spinlock 2.2-arm-lock arm平台使用ticket算法来做，类似于叫号的算法。
https://app.yinxiang.com/shard/s65/nl/15273355/e6b21781-3aab-4f5a-b53c-fc716c6dbf0f/
*/
typedef struct {
	union {
		u32 slock;
		struct __raw_tickets {
#ifdef __ARMEB__
			u16 next;
			u16 owner;
#else
			u16 owner;
			u16 next;
#endif
		} tickets;
	};
} arch_spinlock_t;

#define __ARCH_SPIN_LOCK_UNLOCKED	{ { 0 } }

typedef struct {
	u32 lock;
} arch_rwlock_t;

#define __ARCH_RW_LOCK_UNLOCKED		{ 0 }

#endif
