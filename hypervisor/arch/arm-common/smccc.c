/*
 * Jailhouse, a Linux-based partitioning hypervisor
 *
 * Copyright (C) 2018 Texas Instruments Incorporated - http://www.ti.com/
 * Copyright 2018 NXP
 *
 * Authors:
 *  Lokesh Vutla <lokeshvutla@ti.com>
 *  Flynn Xu<flynn.xu@nxp.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <jailhouse/control.h>
#include <asm/psci.h>
#include <asm/traps.h>
#include <asm/smccc.h>

static void trust_dispatch(struct trap_context *ctx)
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

static long handle_arch(struct trap_context *ctx)
{
	u32 function_id = ctx->regs[0];

	switch (function_id) {
	case SMCCC_VERSION:
		return ARM_SMCCC_VERSION_1_1;

	/* No features supported yet */
	case SMCCC_ARCH_FEATURES:
	default:
		return ARM_SMCCC_NOT_SUPPORTED;
	}
}

long __attribute__((weak)) sip_dispatch(struct trap_context *ctx)
{
	return ARM_SMCCC_NOT_SUPPORTED;
}

enum trap_return handle_smc(struct trap_context *ctx)
{
	unsigned long *regs = ctx->regs;
	u32 *stats = this_cpu_public()->stats;

	switch (SMCCC_GET_OWNER(regs[0])) {
	case ARM_SMCCC_OWNER_ARCH:
		stats[JAILHOUSE_CPU_STAT_VMEXITS_SMCCC]++;
		regs[0] = handle_arch(ctx);
		break;

	case ARM_SMCCC_OWNER_SIP:
		stats[JAILHOUSE_CPU_STAT_VMEXITS_SMCCC]++;
		regs[0] = sip_dispatch(ctx);
		break;

	case ARM_SMCCC_OWNER_STANDARD:
		regs[0] = psci_dispatch(ctx);
		break;

	case ARM_SMCCC_OWNER_TRUSTED_APP ... ARM_SMCCC_OWNER_TRUSTED_APP_END:
	case ARM_SMCCC_OWNER_TRUSTED_OS ... ARM_SMCCC_OWNER_TRUSTED_OS_END:
		trust_dispatch(ctx);
		break;

	default:
		return TRAP_UNHANDLED;

	}

	arch_skip_instruction(ctx);

	return TRAP_HANDLED;
}
