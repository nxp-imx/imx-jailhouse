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

#include <asm/traps.h>

#define SMCCC_VERSION			0x80000000
#define SMCCC_ARCH_FEATURES		0x80000001

#define ARM_SMCCC_OWNER_MASK		BIT_MASK(29, 24)
#define ARM_SMCCC_OWNER_SHIFT		24

#define ARM_SMCCC_OWNER_ARCH		0
#define ARM_SMCCC_OWNER_SIP             2
#define ARM_SMCCC_OWNER_STANDARD        4
#define ARM_SMCCC_OWNER_TRUSTED_APP		48
#define ARM_SMCCC_OWNER_TRUSTED_APP_END 49
#define ARM_SMCCC_OWNER_TRUSTED_OS		50
#define ARM_SMCCC_OWNER_TRUSTED_OS_END  63

#define ARM_SMCCC_CONV_32		0
#define ARM_SMCCC_CONV_64		1

#define ARM_SMCCC_NOT_SUPPORTED         (-1)
#define ARM_SMCCC_SUCCESS               0

#define ARM_SMCCC_VERSION_1_1		0x10001

#define SMCCC_GET_OWNER(id)	((id & ARM_SMCCC_OWNER_MASK) >>	\
				 ARM_SMCCC_OWNER_SHIFT)

#define SMCCC_IS_CONV_64(function_id)	!!(function_id & (1 << 30))

int handle_smc(struct trap_context *ctx);
long sip_dispatch(struct trap_context *ctx);
unsigned long call_smcc64(unsigned long fid, unsigned long a0,
			  unsigned long a1, unsigned long a2, unsigned long a3,
			  unsigned long a4, unsigned long a5,
			  unsigned long *ret);
