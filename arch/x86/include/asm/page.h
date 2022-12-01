/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_PAGE_H
#define _ASM_X86_PAGE_H

#include <linux/types.h>

#ifdef __KERNEL__

#include <asm/page_types.h>

#ifdef CONFIG_X86_64
#include <asm/page_64.h>
#else
#include <asm/page_32.h>
#endif	/* CONFIG_X86_64 */

#ifndef __ASSEMBLY__

struct page;

#include <linux/range.h>
extern struct range pfn_mapped[];
extern int nr_pfn_mapped;

static inline void clear_user_page(void *page, unsigned long vaddr,
				   struct page *pg)
{
	clear_page(page);
}

static inline void copy_user_page(void *to, void *from, unsigned long vaddr,
				  struct page *topage)
{
	copy_page(to, from);
}

#define __alloc_zeroed_user_highpage(movableflags, vma, vaddr) \
	alloc_page_vma(GFP_HIGHUSER | __GFP_ZERO | movableflags, vma, vaddr)
#define __HAVE_ARCH_ALLOC_ZEROED_USER_HIGHPAGE

#ifndef __pa

/*xiaojin-mm-data-page __pa(x)红包装，获得虚拟地址的物理地址。*/
#define __pa(x)		__phys_addr((unsigned long)(x))
#endif

#define __pa_nodebug(x)	__phys_addr_nodebug((unsigned long)(x))
/* __pa_symbol should be used for C visible symbols.
   This seems to be the official gcc blessed way to do such arithmetic. */
/*
 * We need __phys_reloc_hide() here because gcc may assume that there is no
 * overflow during __pa() calculation and can optimize it unexpectedly.
 * Newer versions of gcc provide -fno-strict-overflow switch to handle this
 * case properly. Once all supported versions of gcc understand it, we can
 * remove this Voodoo magic stuff. (i.e. once gcc3.x is deprecated)
 */
#define __pa_symbol(x) \
	__phys_addr_symbol(__phys_reloc_hide((unsigned long)(x)))

#ifndef __va
#define __va(x)			((void *)((unsigned long)(x)+PAGE_OFFSET))
#endif

#define __boot_va(x)		__va(x)
#define __boot_pa(x)		__pa(x)

/*
 * virt_to_page(kaddr) returns a valid pointer if and only if
 * virt_addr_valid(kaddr) returns true.
 */
/*xiaojin-mm-page-ds pfn_to_page pfn_to_kaddr (exp)原理解释-pfn的算法是固定的：这个很重要，pfn- page frame number就是物理页框的页框号，这个是个固定算法值，跟内存模型无关，不同内存模型就是为了适应这个算法而改变不同的结构（参考：memory-modle.rst https://app.yinxiang.com/shard/s65/nl/15273355/14437aa5-383a-4de4-be36-74acb4f519e0）。算法就是：物理地址>>PAGE_SHIFT
*/
#define virt_to_page(kaddr)	pfn_to_page(__pa(kaddr) >> PAGE_SHIFT)
#define pfn_to_kaddr(pfn)      __va((pfn) << PAGE_SHIFT)
extern bool __virt_addr_valid(unsigned long kaddr);
#define virt_addr_valid(kaddr)	__virt_addr_valid((unsigned long) (kaddr))

#endif	/* __ASSEMBLY__ */

#include <asm-generic/memory_model.h>
#include <asm-generic/getorder.h>

#define HAVE_ARCH_HUGETLB_UNMAPPED_AREA

#endif	/* __KERNEL__ */
#endif /* _ASM_X86_PAGE_H */
