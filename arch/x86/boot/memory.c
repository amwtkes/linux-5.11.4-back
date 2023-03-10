// SPDX-License-Identifier: GPL-2.0-only
/* -*- linux-c -*- ------------------------------------------------------- *
 *
 *   Copyright (C) 1991, 1992 Linus Torvalds
 *   Copyright 2007 rPath, Inc. - All Rights Reserved
 *   Copyright 2009 Intel Corporation; author H. Peter Anvin
 *
 * ----------------------------------------------------------------------- */

/*
 * Memory detection code
 */

#include "boot.h"

#define SMAP	0x534d4150	/* ASCII "SMAP" */

static void detect_memory_e820(void)
{
	int count = 0;
	struct biosregs ireg, oreg;
	struct boot_e820_entry *desc = boot_params.e820_table;
	static struct boot_e820_entry buf; /* static so it is zeroed */

	initregs(&ireg);
	ireg.ax  = 0xe820;
	ireg.cx  = sizeof(buf);
	ireg.edx = SMAP;
	ireg.di  = (size_t)&buf;

	/*
	 * Note: at least one BIOS is known which assumes that the
	 * buffer pointed to by one e820 call is the same one as
	 * the previous call, and only changes modified fields.  Therefore,
	 * we use a temporary buffer and copy the results entry by entry.
	 *
	 * This routine deliberately does not try to account for
	 * ACPI 3+ extended attributes.  This is because there are
	 * BIOSes in the field which report zero for the valid bit for
	 * all ranges, and we don't currently make any use of the
	 * other attribute bits.  Revisit this if we see the extended
	 * attribute bits deployed in a meaningful way in the future.
	 */

	/*xiaojin-mm-e820 -1 (impo) 调用bios int 15，填充boot_params.e820_table结构。调用128次，由于BIOS中规定最多只有128个 memory map，所以在kernel 启动时 最多轮询读取128次。
	参考：https://app.yinxiang.com/shard/s65/nl/15273355/b3ad5870-33d4-475a-a6c4-4a97dd5a4752/
	https://app.yinxiang.com/shard/s65/nl/15273355/dc60a45f-7834-4c5c-98bb-803484311d3f/

	设置：struct e820_table *e820_table这个结构，在e820.c里面定义。
	*/
	do {
		//发送中断调用bios程序获取memey map信息
		intcall(0x15, &ireg, &oreg);
		ireg.ebx = oreg.ebx; /* for next iteration... */

		/* BIOSes which terminate the chain with CF = 1 as opposed
		   to %ebx = 0 don't always report the SMAP signature on
		   the final, failing, probe. */
		if (oreg.eflags & X86_EFLAGS_CF)
			break;

		/* Some BIOSes stop returning SMAP in the middle of
		   the search loop.  We don't know exactly how the BIOS
		   screwed up the map at that point, we might have a
		   partial map, the full map, or complete garbage, so
		   just return failure. */
		if (oreg.eax != SMAP) {
			count = 0;
			break;
		}
	/*
		每次调用intcall从0x15中读取一个memory map。
		接下来将本次读取 到的ebx 保存到下一次中ireg.ebx = oreg.ebx，方便下次读取。
		对carry flag状态进行检查，如不被置位则说明出现错误不再进行读取。
		对eax进行验证是否等于SMAP。
		将读取到的memory 信息保存到*desc++ = buf;最终是保存到boot_params.e820_table中。
		count++计数+1。
		如果irg.ebx为零 或者count大于128，则说明读取完毕。
		boot_params.e820_entries记录有多少个memory 信息。
	*/
		*desc++ = buf;
		count++;
	} while (ireg.ebx && count < ARRAY_SIZE(boot_params.e820_table));

	boot_params.e820_entries = count;
}

static void detect_memory_e801(void)
{
	struct biosregs ireg, oreg;

	initregs(&ireg);
	ireg.ax = 0xe801;
	intcall(0x15, &ireg, &oreg);

	if (oreg.eflags & X86_EFLAGS_CF)
		return;

	/* Do we really need to do this? */
	if (oreg.cx || oreg.dx) {
		oreg.ax = oreg.cx;
		oreg.bx = oreg.dx;
	}

	if (oreg.ax > 15*1024) {
		return;	/* Bogus! */
	} else if (oreg.ax == 15*1024) {
		boot_params.alt_mem_k = (oreg.bx << 6) + oreg.ax;
	} else {
		/*
		 * This ignores memory above 16MB if we have a memory
		 * hole there.  If someone actually finds a machine
		 * with a memory hole at 16MB and no support for
		 * 0E820h they should probably generate a fake e820
		 * map.
		 */
		boot_params.alt_mem_k = oreg.ax;
	}
}

static void detect_memory_88(void)
{
	struct biosregs ireg, oreg;

	initregs(&ireg);
	ireg.ah = 0x88;
	intcall(0x15, &ireg, &oreg);

	boot_params.screen_info.ext_mem_k = oreg.ax;
}

void detect_memory(void)
{
	/*xiaojin-mm-e820 -0.1 调用点*/
	detect_memory_e820();

//后面这两种内存探测。可能是支持以前的老机器的兼容代码。
//参考：https://app.yinxiang.com/shard/s65/nl/15273355/966c16d3-9945-45cb-afc6-7e8a41076e4d/
	detect_memory_e801();

	detect_memory_88();
}
