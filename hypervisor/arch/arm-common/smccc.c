/*
 * Jailhouse, a Linux-based partitioning hypervisor
 *
 * Copyright (C) 2018 Texas Instruments Incorporated - http://www.ti.com/
 * Copyright 2019 NXP
 *
 * Authors:
 *  Lokesh Vutla <lokeshvutla@ti.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <jailhouse/control.h>
#include <jailhouse/printk.h>
#include <asm/psci.h>
#include <asm/traps.h>
#include <asm/smc.h>
#include <asm/smccc.h>

static void trust_dispatch(struct trap_context *ctx)
{
#ifdef __aarch64__
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
#endif
}

void smccc_discover(void)
{
	int ret;

	ret = smc(PSCI_0_2_FN_VERSION);

	/* We need >=PSCIv1.0 for SMCCC. Against the spec, U-Boot may also
	 * return a negative error code. */
	if (ret < 0 || PSCI_VERSION_MAJOR(ret) < 1)
		return;

	/* Check if PSCI supports SMCCC version call */
	ret = smc_arg1(PSCI_1_0_FN_FEATURES, SMCCC_VERSION);
	if (ret != ARM_SMCCC_SUCCESS)
		return;

	/* We need to have SMCCC v1.1 */
	ret = smc(SMCCC_VERSION);
	if (ret != ARM_SMCCC_VERSION_1_1)
		return;

	/* check if SMCCC_ARCH_FEATURES is actually available */
	ret = smc_arg1(SMCCC_ARCH_FEATURES, SMCCC_ARCH_FEATURES);
	if (ret != ARM_SMCCC_SUCCESS)
		return;

	ret = smc_arg1(SMCCC_ARCH_FEATURES, SMCCC_ARCH_WORKAROUND_1);

	this_cpu_data()->smccc_has_workaround_1 = ret >= ARM_SMCCC_SUCCESS;
}

static inline long handle_arch_features(u32 id)
{
	switch (id) {
	case SMCCC_ARCH_FEATURES:
		return ARM_SMCCC_SUCCESS;

	case SMCCC_ARCH_WORKAROUND_1:
		return this_cpu_data()->smccc_has_workaround_1 ?
			ARM_SMCCC_SUCCESS : ARM_SMCCC_NOT_SUPPORTED;

	default:
		return ARM_SMCCC_NOT_SUPPORTED;
	}
}

static enum trap_return handle_arch(struct trap_context *ctx)
{
	u32 function_id = ctx->regs[0];
	unsigned long *ret = &ctx->regs[0];

	switch (function_id) {
	case SMCCC_VERSION:
		*ret = ARM_SMCCC_VERSION_1_1;
		break;

	case SMCCC_ARCH_FEATURES:
		*ret = handle_arch_features(ctx->regs[1]);
		break;

	default:
		panic_printk("Unhandled SMC arch trap %lx\n", *ret);
		return TRAP_UNHANDLED;
	}

	return TRAP_HANDLED;
}

long __attribute__((weak)) sip_dispatch(struct trap_context *ctx)
{
	return ARM_SMCCC_NOT_SUPPORTED;
}

enum trap_return handle_smc(struct trap_context *ctx)
{
	unsigned long *regs = ctx->regs;
	enum trap_return ret = TRAP_HANDLED;
	u32 *stats = this_cpu_public()->stats;

	switch (SMCCC_GET_OWNER(regs[0])) {
	case ARM_SMCCC_OWNER_ARCH:
		stats[JAILHOUSE_CPU_STAT_VMEXITS_SMCCC]++;
		ret = handle_arch(ctx);
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
		ret = TRAP_UNHANDLED;
	}

	arch_skip_instruction(ctx);

	return ret;
}
