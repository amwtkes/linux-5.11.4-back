/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_PAGE_DEFS_H
#define _ASM_X86_PAGE_DEFS_H

#include <linux/const.h>
#include <linux/types.h>
#include <linux/mem_encrypt.h>

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT		12
#define PAGE_SIZE		(_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE-1))

#define PMD_PAGE_SIZE		(_AC(1, UL) << PMD_SHIFT) //PMD_SHIFT=21 PMD_PAGE_SIZE=1后面21个0
#define PMD_PAGE_MASK		(~(PMD_PAGE_SIZE-1)) //PMD_PAGE_MASK=21个0，高位都是1

#define PUD_PAGE_SIZE		(_AC(1, UL) << PUD_SHIFT)
#define PUD_PAGE_MASK		(~(PUD_PAGE_SIZE-1)) //PUD_PAGE_MASK=21个0，其余都是1

#define __VIRTUAL_MASK		((1UL << __VIRTUAL_MASK_SHIFT) - 1)

/* Cast *PAGE_MASK to a signed type so that it is sign-extended if
   virtual addresses are 32-bits but physical addresses are larger
   (ie, 32-bit PAE). */
#define PHYSICAL_PAGE_MASK	(((signed long)PAGE_MASK) & __PHYSICAL_MASK)
/*xiaojin-mm-sparsemem-pagetable (exp)原理解释——屏蔽字段的mask怎么解读？__PHYSICAL_MASK_SHIFT=52 __PHYSICAL_MASK=52个1高位都是0 PHYSICAL_PMD_PAGE_MASK=0X 000f ffff ffe0 0000 中间31个1，目的是截取前52位（52位有效地址，intel规定）中除开PTE=9与page中的offset=12位，一共21位以外的31位。这说明，这是一个大页2MB=2^21的大页*/
#define PHYSICAL_PMD_PAGE_MASK	(((signed long)PMD_PAGE_MASK) & __PHYSICAL_MASK) 
#define PHYSICAL_PUD_PAGE_MASK	(((signed long)PUD_PAGE_MASK) & __PHYSICAL_MASK)

#define HPAGE_SHIFT		PMD_SHIFT
#define HPAGE_SIZE		(_AC(1,UL) << HPAGE_SHIFT)
#define HPAGE_MASK		(~(HPAGE_SIZE - 1))
#define HUGETLB_PAGE_ORDER	(HPAGE_SHIFT - PAGE_SHIFT)

#define HUGE_MAX_HSTATE 2

#define PAGE_OFFSET		((unsigned long)__PAGE_OFFSET)

#define VM_DATA_DEFAULT_FLAGS	VM_DATA_FLAGS_TSK_EXEC

#define __PHYSICAL_START	ALIGN(CONFIG_PHYSICAL_START, \
				      CONFIG_PHYSICAL_ALIGN)

/*xiaojin-mm-data-page __START_KERNEL __START_KERNEL_map + 0x1000000 = 0xffffffff81000000  __PHYSICAL_START物理地址是 0x1000000 16M的物理地址位置。注意：1000就是一页=4k
参考：https://app.yinxiang.com/shard/s65/nl/15273355/72d5ccd1-22fd-4d93-b186-b7476c5c305c/
     https://app.yinxiang.com/shard/s65/nl/15273355/34fa295d-c2cc-43fc-8500-ceebb64b2c37/
*/
#define __START_KERNEL		(__START_KERNEL_map + __PHYSICAL_START)

#ifdef CONFIG_X86_64
#include <asm/page_64_types.h>
#define IOREMAP_MAX_ORDER       (PUD_SHIFT)
#else
#include <asm/page_32_types.h>
#define IOREMAP_MAX_ORDER       (PMD_SHIFT)
#endif	/* CONFIG_X86_64 */

#ifndef __ASSEMBLY__

#ifdef CONFIG_DYNAMIC_PHYSICAL_MASK
extern phys_addr_t physical_mask;
#define __PHYSICAL_MASK		physical_mask
#else
#define __PHYSICAL_MASK		((phys_addr_t)((1ULL << __PHYSICAL_MASK_SHIFT) - 1))
#endif

extern int devmem_is_allowed(unsigned long pagenr);

extern unsigned long max_low_pfn_mapped;
extern unsigned long max_pfn_mapped;

static inline phys_addr_t get_max_mapped(void)
{
	return (phys_addr_t)max_pfn_mapped << PAGE_SHIFT;
}

bool pfn_range_is_mapped(unsigned long start_pfn, unsigned long end_pfn);

extern void initmem_init(void);

#endif	/* !__ASSEMBLY__ */

#endif	/* _ASM_X86_PAGE_DEFS_H */
