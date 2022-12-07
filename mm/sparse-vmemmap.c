// SPDX-License-Identifier: GPL-2.0
/*
 * Virtual Memory Map support
 *
 * (C) 2007 sgi. Christoph Lameter.
 *
 * Virtual memory maps allow VM primitives pfn_to_page, page_to_pfn,
 * virt_to_page, page_address() to be implemented as a base offset
 * calculation without memory access.
 *
 * However, virtual mappings need a page table and TLBs. Many Linux
 * architectures already map their physical space using 1-1 mappings
 * via TLBs. For those arches the virtual memory map is essentially
 * for free if we use the same page size as the 1-1 mappings. In that
 * case the overhead consists of a few additional pages that are
 * allocated to create a view of memory for vmemmap.
 *
 * The architecture is expected to provide a vmemmap_populate() function
 * to instantiate the mapping.
 */
#include <linux/mm.h>
#include <linux/mmzone.h>
#include <linux/memblock.h>
#include <linux/memremap.h>
#include <linux/highmem.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <asm/dma.h>
#include <asm/pgalloc.h>

/*
 * Allocate a block of memory to be used to back the virtual memory map
 * or to back the page tables that are used to create the mapping.
 * Uses the main allocators if they are available, else bootmem.
 */

static void * __ref __earlyonly_bootmem_alloc(int node,
				unsigned long size,
				unsigned long align,
				unsigned long goal)
{
	return memblock_alloc_try_nid_raw(size, align, goal,
					       MEMBLOCK_ALLOC_ACCESSIBLE, node);
}

void * __meminit vmemmap_alloc_block(unsigned long size, int node)
{
	/* If the main allocator is up use that, fallback to bootmem. */
	if (slab_is_available()) {
		gfp_t gfp_mask = GFP_KERNEL|__GFP_RETRY_MAYFAIL|__GFP_NOWARN;
		int order = get_order(size);
		static bool warned;
		struct page *page;

		page = alloc_pages_node(node, gfp_mask, order);
		if (page)
		//将物理页框的虚拟地址存入page结构的virtual字段中。
			return page_address(page);

		if (!warned) {
			warn_alloc(gfp_mask & ~__GFP_NOWARN, NULL,
				   "vmemmap alloc failure: order:%u", order);
			warned = true;
		}
		return NULL;
	} else
		return __earlyonly_bootmem_alloc(node, size, size,
				__pa(MAX_DMA_ADDRESS));
}

static void * __meminit altmap_alloc_block_buf(unsigned long size,
					       struct vmem_altmap *altmap);

/* need to make sure size is all the same during early stage */
void * __meminit vmemmap_alloc_block_buf(unsigned long size, int node,
					 struct vmem_altmap *altmap)
{
	void *ptr;

	if (altmap)
		return altmap_alloc_block_buf(size, altmap);

	ptr = sparse_buffer_alloc(size);
	if (!ptr)
		ptr = vmemmap_alloc_block(size, node);
	return ptr;
}

static unsigned long __meminit vmem_altmap_next_pfn(struct vmem_altmap *altmap)
{
	return altmap->base_pfn + altmap->reserve + altmap->alloc
		+ altmap->align;
}

static unsigned long __meminit vmem_altmap_nr_free(struct vmem_altmap *altmap)
{
	unsigned long allocated = altmap->alloc + altmap->align;

	if (altmap->free > allocated)
		return altmap->free - allocated;
	return 0;
}

static void * __meminit altmap_alloc_block_buf(unsigned long size,
					       struct vmem_altmap *altmap)
{
	unsigned long pfn, nr_pfns, nr_align;

	if (size & ~PAGE_MASK) {
		pr_warn_once("%s: allocations must be multiple of PAGE_SIZE (%ld)\n",
				__func__, size);
		return NULL;
	}

	pfn = vmem_altmap_next_pfn(altmap);
	nr_pfns = size >> PAGE_SHIFT;
	nr_align = 1UL << find_first_bit(&nr_pfns, BITS_PER_LONG);
	nr_align = ALIGN(pfn, nr_align) - pfn;
	if (nr_pfns + nr_align > vmem_altmap_nr_free(altmap))
		return NULL;

	altmap->alloc += nr_pfns;
	altmap->align += nr_align;
	pfn += nr_align;

	pr_debug("%s: pfn: %#lx alloc: %ld align: %ld nr: %#lx\n",
			__func__, pfn, altmap->alloc, altmap->align, nr_pfns);
	return __va(__pfn_to_phys(pfn));
}

void __meminit vmemmap_verify(pte_t *pte, int node,
				unsigned long start, unsigned long end)
{
	unsigned long pfn = pte_pfn(*pte);
	int actual_node = early_pfn_to_nid(pfn);

	if (node_distance(actual_node, node) > LOCAL_DISTANCE)
		pr_warn("[%lx-%lx] potential offnode page_structs\n",
			start, end - 1);
}
/*xiaojin-mm-pagetable -4.1 形成pte的函数。vmemmap_pte_populate */
pte_t * __meminit vmemmap_pte_populate(pmd_t *pmd, unsigned long addr, int node,
				       struct vmem_altmap *altmap)
{
	/*xiaojin-mm-pagetable -4.2.0 调用点*/
	pte_t *pte = pte_offset_kernel(pmd, addr);
	if (pte_none(*pte)) {
		pte_t entry;
		void *p; // 这里的p是page frame的线性地址
		//如果pte没有值，则分配一个页做物理内存，然后映射到这个pte上面。
		p = vmemmap_alloc_block_buf(PAGE_SIZE, node, altmap);
		if (!p)
			return NULL;
		/*
		#define __PAGE_KERNEL		 (__PP|__RW|   0|___A|__NX|___D|   0|___G)
		表示是个kernel的映射。因为第三个bit是0.
		*/
	/*xiaojin-mm-pagetable -4.1.1 pfn_pte调用处*/	
		entry = pfn_pte(__pa(p) >> PAGE_SHIFT/*得到pfn*/, PAGE_KERNEL); 
		/*xiaojin-mm-pagetable -4.3 分配完物理页以后，将这个页号放入也表项中*/
		set_pte_at(&init_mm, addr, pte, entry);
	}
	return pte;
}

static void * __meminit vmemmap_alloc_block_zero(unsigned long size, int node)
{
	void *p = vmemmap_alloc_block(size, node);

	if (!p)
		return NULL;
	memset(p, 0, size);

	return p;
}

pmd_t * __meminit vmemmap_pmd_populate(pud_t *pud, unsigned long addr, int node)
{
	pmd_t *pmd = pmd_offset(pud, addr);
	if (pmd_none(*pmd)) {
		void *p = vmemmap_alloc_block_zero(PAGE_SIZE, node);
		if (!p)
			return NULL;
		pmd_populate_kernel(&init_mm, pmd, p);
	}
	return pmd;
}

/*xiaojin-mm-sparsemem -2.2.1.1 vmemmap_pud_populate 定义.操作页表*/
/*xiaojin-mm-pagetable -2 vmemmap_pud_populate*/
pud_t * __meminit vmemmap_pud_populate(p4d_t *p4d, unsigned long addr, int node)
{
	pud_t *pud = pud_offset(p4d, addr);
	if (pud_none(*pud)) { //如果addr的在pgd上的偏移（4层页表p4d==pgd）没有设置，就要重新分配一个页做pmd的目录。

		//pmd目录页的虚拟地址。因为是在直接映射区所以就是物理地址+page_off的偏移。
		void *p = vmemmap_alloc_block_zero(PAGE_SIZE, node);
		if (!p)
			return NULL;
		/*xiaojin-mm-pagetable -3.0 调用pud_populate*/
		pud_populate(&init_mm, pud, p);//pud是pud页表项的线性地址，p是pmd页的线性地址。
	}
	return pud;
}

p4d_t * __meminit vmemmap_p4d_populate(pgd_t *pgd, unsigned long addr, int node)
{
	p4d_t *p4d = p4d_offset(pgd, addr);
	if (p4d_none(*p4d)) {
		void *p = vmemmap_alloc_block_zero(PAGE_SIZE, node);
		if (!p)
			return NULL;
		p4d_populate(&init_mm, p4d, p);
	}
	return p4d;
}

/*xiaojin-mm-pagetable -1 vmemmap_pgd_populate
*/
pgd_t * __meminit vmemmap_pgd_populate(unsigned long addr, int node)
{
	pgd_t *pgd = pgd_offset_k(addr);
	//pglevel小于4是0
	if (pgd_none(*pgd)) {
		void *p = vmemmap_alloc_block_zero(PAGE_SIZE, node);
		if (!p)
			return NULL;
		pgd_populate(&init_mm, pgd, p);
	}
	return pgd;
}

/*xiaojin-mm-sparsemem -2.2.1 如何映射页表来创建mem_map的struct page* 数组。操作页表。
xiaojin-mm-pagetable (exp)原理解释——（如何建立页表映射？）为什么建立页表只要知道线性地址范围即可？具体的函数目的是：为start - end这个线性地址区域建立页表映射。为啥因为在内核，线性地址与物理地址可以简单的通过__pa __va进行转换，是直接映射的，也可以说是连续简单映射的，所以只要一个就知道另一个。所以在这里建立内核页映射只要知道线性地址范围就行了。那为什么要映射呢？因为开启页寻址功能的intel CPU必须通过页表+mmu来寻址，不管用户态还是内核态。映射建立其实就是做好struct page结构去描述被映射的物理内存地址。
*/
int __meminit vmemmap_populate_basepages(unsigned long start, unsigned long end,
					 int node, struct vmem_altmap *altmap)
{
	unsigned long addr = start;
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	for (; addr < end; addr += PAGE_SIZE) {
		/*xiaojin-mm-pagetable -example sparsemem
		addr是虚拟地址，是一个页面的起始地址。
		*/

		//拿pgd
		pgd = vmemmap_pgd_populate(addr, node);
		if (!pgd)
			return -ENOMEM;
		
		//4层页表返回的值就是pgd
		p4d = vmemmap_p4d_populate(pgd, addr, node);
		if (!p4d)
			return -ENOMEM;
		/*xiaojin-mm-sparsemem -2.2.1.1 初始化upper page table entry*/
		pud = vmemmap_pud_populate(p4d, addr, node);
		if (!pud)
			return -ENOMEM;
		pmd = vmemmap_pmd_populate(pud, addr, node);
		if (!pmd)
			return -ENOMEM;
		/*xiaojin-mm-pagetable -4.0 调用点*/
		pte = vmemmap_pte_populate(pmd, addr, node, altmap);
		if (!pte)
			return -ENOMEM;
		vmemmap_verify(pte, node, addr, addr + PAGE_SIZE);
	}

	return 0;
}

/*xiaojin-mm-sparsemem -2.2 使用vmemmap的配置，初始化section的mem_map。pfn是这个内存区域开始的物理内存页框号，nr_pages是这个内存区域一共有多少个页框，从而根据这个计算出这个section内的最后一个页的线性地址。
*/
struct page * __meminit __populate_section_memmap(unsigned long pfn,
		unsigned long nr_pages, int nid, struct vmem_altmap *altmap)
{
	/*xiaojin-mm-sparsemem (exp)原理解释——如何使用vmemmap？vmemmap不是计算出来的，是定义的，它可以是任何线性地址，只要满足一个公式PFN = page页描述符的线性地址 - vmemmap就可以了。其中PFN由物理页框物理地址确定，page是内核线性地址映射的，所以这两个变量是没有关系的，所以vmemmap在哪就是原则上可以是随意的。只是有一点要注意，vmemmap是虚拟的数组，不能当做数组来使用，不能取值，只能做pfn_to_page与page_to_pfn这种计算。这是跟FLAT模型不同的。

	从下面这行代码也可以看到，start就是section的mem_map起始地址，是由vmemmap+pfn得到的(而不是先映射再计算vmemmap！！！)，印证了上面的公式。start是section_memmap的起始页（肯定是页对齐的）的线性地址=pfn（页框号）+ vmemmap。

	对于物理内存到虚拟内存映射的问题只要形成page结构就行了，物理内存只是个范围，不需要做什么操作，page本身通过vmemmap定位到，然后就可以通过page_to_pfn得到相应的物理内存范围，因此，内存映射只需要生成page对象，填好相应的信息就行，这也是为啥struct page结构叫做物理页框描述符的原因了。

	还有，这是是系统初始化的函数，会将每个node下的section映射到内核的线性地址中.分配映射其实就是形成struct page结构去描述这段物理内存而已。
	*/

/*xiaojin-mm-pagetable -example0 分配内存，只要确定了虚拟地址范围，就可以分配了，大块内存一般是以页为单位分配内存，小块的brk()分配。这里start是分配线性地址的起始页位置，end是结束页地址。vmemmap_populate函数进行页表映射。*/
	unsigned long start = (unsigned long) pfn_to_page(pfn);
	unsigned long end = start + nr_pages * sizeof(struct page);

	if (WARN_ON_ONCE(!IS_ALIGNED(pfn, PAGES_PER_SUBSECTION) ||
		!IS_ALIGNED(nr_pages, PAGES_PER_SUBSECTION)))
		return NULL;

	if (vmemmap_populate(start, end, nid, altmap))
		return NULL;

	return pfn_to_page(pfn);
}
