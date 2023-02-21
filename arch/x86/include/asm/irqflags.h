/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _X86_IRQFLAGS_H_
#define _X86_IRQFLAGS_H_

#include <asm/processor-flags.h>

#ifndef __ASSEMBLY__

#include <asm/nospec-branch.h>

/* Provide __cpuidle; we can't safely include <linux/cpu.h> */
#define __cpuidle __section(".cpuidle.text")

/*
 * Interrupt control:
 */

/* Declaration required for gcc < 4.9 to prevent -Werror=missing-prototypes */
extern inline unsigned long native_save_fl(void);
extern __always_inline unsigned long native_save_fl(void)
{
	unsigned long flags;

	/*
	 * "=rm" is safe here, because "pop" adjusts the stack before
	 * it evaluates its effective address -- this is part of the
	 * documented behavior of the "pop" instruction.
	 */
	asm volatile("# __raw_save_flags\n\t"
		     "pushf ; pop %0"
		     : "=rm" (flags)
		     : /* no input */
		     : "memory");

	return flags;
}

extern inline void native_restore_fl(unsigned long flags);

/*xiaojin-interrupt -raw_local_irq_restore -2 用flags的值刷到eflags寄存器中。使用popf指令。

*/
extern inline void native_restore_fl(unsigned long flags)
{
	asm volatile("push %0 ; popf"
		     : /* no output */
		     :"g" (flags)
		     :"memory", "cc");
}

/*xiaojin-interrupt -rq_disable 关闭中断 native_irq_disable设置rflags寄存器的IF位为0来将maskable的中断屏蔽掉。
*/
static __always_inline void native_irq_disable(void)
{
	asm volatile("cli": : :"memory");
}

static __always_inline void native_irq_enable(void)
{
	asm volatile("sti": : :"memory");
}

static inline __cpuidle void native_safe_halt(void)
{
	mds_idle_clear_cpu_buffers();
	asm volatile("sti; hlt": : :"memory");
}

static inline __cpuidle void native_halt(void)
{
	mds_idle_clear_cpu_buffers();
	asm volatile("hlt": : :"memory");
}

#endif

#ifdef CONFIG_PARAVIRT_XXL
#include <asm/paravirt.h>
#else
#ifndef __ASSEMBLY__
#include <linux/types.h>

static __always_inline unsigned long arch_local_save_flags(void)
{
	return native_save_fl();
}

/*xiaojin-interrupt -raw_local_irq_restore -1 arch_local_irq_restore
*/
static __always_inline void arch_local_irq_restore(unsigned long flags)
{
	native_restore_fl(flags);
}

/*xiaojin-interrupt -rq_disable arch_local_irq_disable (exp)回答如果CPU在陷入中断的时候已经关闭了中断（防止重入）那么这个手动的关闭中断的函数干嘛的呢？这里只在手动保存rflags的时候会主动关闭终端。因为intel cpu会在陷入中断的时候自动将rflags寄存器压入栈中。只在一个地方用了——arch_local_irq_save()保存rflags寄存器的时候要禁止中断。
*/
static __always_inline void arch_local_irq_disable(void)
{
	native_irq_disable();
}
static __always_inline void arch_local_irq_enable(void)
{
	native_irq_enable();
}

/*
 * Used in the idle loop; sti takes one instruction cycle
 * to complete:
 */
static inline __cpuidle void arch_safe_halt(void)
{
	native_safe_halt();
}

/*
 * Used when interrupts are already enabled or to
 * shutdown the processor:
 */
static inline __cpuidle void halt(void)
{
	native_halt();
}

/*
 * For spinlocks, etc:
 */
/*xiaojin-interrupt arch_local_irq_save arch_local_save_flags ---- Push EFLAGS Register onto the Stack
参考：https://app.yinxiang.com/shard/s65/nl/15273355/4492290c-dcd0-4c00-a6f7-506f9cfea2fd
保存eflag的同时要关闭中断*/
static __always_inline unsigned long arch_local_irq_save(void)
{
	unsigned long flags = arch_local_save_flags();
	arch_local_irq_disable();
	return flags;
}
#else

#define ENABLE_INTERRUPTS(x)	sti
#define DISABLE_INTERRUPTS(x)	cli

#ifdef CONFIG_X86_64
#ifdef CONFIG_DEBUG_ENTRY
#define SAVE_FLAGS(x)		pushfq; popq %rax
#endif

#define SWAPGS	swapgs
/*
 * Currently paravirt can't handle swapgs nicely when we
 * don't have a stack we can rely on (such as a user space
 * stack).  So we either find a way around these or just fault
 * and emulate if a guest tries to call swapgs directly.
 *
 * Either way, this is a good way to document that we don't
 * have a reliable stack. x86_64 only.
 */
#define SWAPGS_UNSAFE_STACK	swapgs

#define INTERRUPT_RETURN	jmp native_iret
#define USERGS_SYSRET64				\
	swapgs;					\
	sysretq;
#define USERGS_SYSRET32				\
	swapgs;					\
	sysretl

#else
#define INTERRUPT_RETURN		iret
#endif

#endif /* __ASSEMBLY__ */
#endif /* CONFIG_PARAVIRT_XXL */

#ifndef __ASSEMBLY__
static __always_inline int arch_irqs_disabled_flags(unsigned long flags)
{
	return !(flags & X86_EFLAGS_IF);
}

static __always_inline int arch_irqs_disabled(void)
{
	unsigned long flags = arch_local_save_flags();

	return arch_irqs_disabled_flags(flags);
}
#endif /* !__ASSEMBLY__ */

#endif
