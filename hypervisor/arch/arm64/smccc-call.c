/*
 * Jailhouse ARM support
 *
 * Copyright 2018 NXP
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <asm/smccc.h>

unsigned long call_smcc64(unsigned long fid, unsigned long a0,
			  unsigned long a1, unsigned long a2,
			  unsigned long a3, unsigned long a4,
			  unsigned long a5, unsigned long *ret)
{
	register unsigned long x0 asm("x0") = fid;
	register unsigned long x1 asm("x1") = a0;
	register unsigned long x2 asm("x2") = a1;
	register unsigned long x3 asm("x3") = a2;
	register unsigned long x4 asm("x4") = a3;
	register unsigned long x5 asm("x5") = a4;
	register unsigned long x6 asm("x6") = a5;

	asm volatile ("smc #0\n"
		      : "+r" (x0), "+r" (x1), "+r" (x2), "+r" (x3),
		      "+r" (x4), "+r" (x5), "+r" (x6)
		      :
		      : "x7", "x8", "x9", "x10", "x11", "x12",
		      "x13", "x14", "x15", "x16", "x17" );

	if (ret) {
		ret[0] = x0;
		ret[1] = x1;
		ret[2] = x2;
		ret[3] = x3;
	}

	return x0;
}
