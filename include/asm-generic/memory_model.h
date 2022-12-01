/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_MEMORY_MODEL_H
#define __ASM_MEMORY_MODEL_H

#include <linux/pfn.h>

#ifndef __ASSEMBLY__

#if defined(CONFIG_FLATMEM)

#ifndef ARCH_PFN_OFFSET
#define ARCH_PFN_OFFSET		(0UL)
#endif

#elif defined(CONFIG_DISCONTIGMEM)

#ifndef arch_pfn_to_nid
#define arch_pfn_to_nid(pfn)	pfn_to_nid(pfn)
#endif

#ifndef arch_local_page_offset
#define arch_local_page_offset(pfn, nid)	\
	((pfn) - NODE_DATA(nid)->node_start_pfn)
#endif

#endif /* CONFIG_DISCONTIGMEM */

/*
 * supports 3 memory models.
 */
#if defined(CONFIG_FLATMEM)
/*xiaojin-mm-page-table-macro __pfn_to_page 从页号获得页描述符的地址*/
#define __pfn_to_page(pfn)	(mem_map + ((pfn) - ARCH_PFN_OFFSET))

/*xiaojin-mm-page-table-macro __page_to_pfn 
*/
#define __page_to_pfn(page)	((unsigned long)((page) - mem_map) + \
				 ARCH_PFN_OFFSET)
#elif defined(CONFIG_DISCONTIGMEM)

#define __pfn_to_page(pfn)			\
({	unsigned long __pfn = (pfn);		\
	unsigned long __nid = arch_pfn_to_nid(__pfn);  \
	NODE_DATA(__nid)->node_mem_map + arch_local_page_offset(__pfn, __nid);\
})

#define __page_to_pfn(pg)						\
({	const struct page *__pg = (pg);					\
	struct pglist_data *__pgdat = NODE_DATA(page_to_nid(__pg));	\/*nid存放在page->flag里面了*/
	(unsigned long)(__pg - __pgdat->node_mem_map) +			\
	 __pgdat->node_start_pfn;					\
})

#elif defined(CONFIG_SPARSEMEM_VMEMMAP)

/* memmap is virtually contiguous.  */
/* xiaojin-mm-sparsemem-ds __page_to_pfn __page_to_pfn*/

#define __pfn_to_page(pfn)	(vmemmap + (pfn))
#define __page_to_pfn(page)	(unsigned long)((page) - vmemmap)

#elif defined(CONFIG_SPARSEMEM)
/*
 * Note: section's mem_map is encoded to reflect its start_pfn.
 * section[i].section_mem_map == mem_map's address - start_pfn;
 */

/*
xiaojin-mm-page-table-macro __pfn_to_page 从页号获得页描述符的地址.(exp)原理解释-页描述符与页框的关系,如何通过pfn来获得页框的物理地址？。（平坦内存模型）根据一个page descriptor获取这个pd的物理页框的页号。就是页框物理地址>>12，因为页框物理地址后12位本来就是0，所以用高位做索引.不管page与mem_map数组是虚拟地址还是物理地址，page struct在mem_map的索引号（pfn）就是物理页框的物理地址的高位值——所谓的页号。所以只要得到这个差值，再右移12位就能得到page frame的物理地址了。
参考：
https://app.yinxiang.com/shard/s65/nl/15273355/c6cc6382-ccb3-4aab-8f09-384296b39a55 

（！！！稀疏内存模型！）为了解决DISCONTIGMEM存在的这些弊端，没过几年，一种新的稀疏内存模型(sparse memory model)被召唤了出来。在SPARSEMEM中，被管理的物理内存由一个个section（用struct mem_section表示）构成，因此整个物理内存可视为一个mem_section数组。
每个mem_section包含一个间接指向struct page数组的指针。为了更有效的实现PFN和struct page之间的转换，PFN中的几个高位bit被用作mem_section数组的索引。
https://app.yinxiang.com/shard/s65/nl/15273355/1b6b7240-ca5e-4552-b69c-b4369d7e1357/
*/
#define __page_to_pfn(pg)					\
({	const struct page *__pg = (pg);				\
	int __sec = page_to_section(__pg);			\
	(unsigned long)(__pg - __section_mem_map_addr(__nr_to_section(__sec)));	\
})

#define __pfn_to_page(pfn)				\
({	unsigned long __pfn = (pfn);			\
	struct mem_section *__sec = __pfn_to_section(__pfn);	\
	__section_mem_map_addr(__sec) + __pfn;		\
})
#endif /* CONFIG_FLATMEM/DISCONTIGMEM/SPARSEMEM */

/*
 * Convert a physical address to a Page Frame Number and back
 */
#define	__phys_to_pfn(paddr)	PHYS_PFN(paddr)
#define	__pfn_to_phys(pfn)	PFN_PHYS(pfn)

#define page_to_pfn __page_to_pfn
#define pfn_to_page __pfn_to_page

#endif /* __ASSEMBLY__ */

#endif
