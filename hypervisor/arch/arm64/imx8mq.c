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
