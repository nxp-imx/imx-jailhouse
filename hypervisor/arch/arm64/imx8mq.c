/*
 * Jailhouse ARM support
 *
 * Copyright 2017 NXP
 *
 * Authors:
 *	Peng Fan <peng.fan@nxp.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <jailhouse/printk.h>
#include <asm/smccc.h>
#include <asm/traps.h>

#define	FSL_SIP_GPC			0xC2000000
#define	FSL_SIP_CONFIG_GPC_PM_DOMAIN	0x03

long sip_dispatch(struct trap_context *ctx)
{
	unsigned long ret[4];

	call_smcc64(ctx->regs[0], ctx->regs[1], ctx->regs[2], ctx->regs[3],
		    ctx->regs[4], ctx->regs[5], ctx->regs[6], ret);

	return ret[0];
}

void trust_dispatch(struct trap_context *ctx)
{
	register unsigned long x0 asm("x0") = ctx->regs[0];
	register unsigned long x1 asm("x1") = ctx->regs[1];
	register unsigned long x2 asm("x2") = ctx->regs[2];
	register unsigned long x3 asm("x3") = ctx->regs[3];
	register unsigned long x4 asm("x4") = ctx->regs[4];
	register unsigned long x5 asm("x5") = ctx->regs[5];
	register unsigned long x6 asm("x6") = ctx->regs[6];
	register unsigned long x7 asm("x7") = ctx->regs[7];

	asm volatile ("smc #0\n"
		      : "+r" (x0), "+r" (x1), "+r" (x2), "+r" (x3),
		      "+r" (x4), "+r" (x5), "+r" (x6), "+r" (x7)
		      :
		      : "x8", "x9", "x10", "x11", "x12",
		      "x13", "x14", "x15", "x16", "x17" );

	ctx->regs[0] = x0;
	ctx->regs[1] = x1;
	ctx->regs[2] = x2;
	ctx->regs[3] = x3;
}
