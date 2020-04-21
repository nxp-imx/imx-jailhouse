/*
 * Jailhouse, a Linux-based partitioning hypervisor
 *
 * Copyright 2018-2020 NXP
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 *
 * Modified from Linux smmu.c
 */

#include <jailhouse/control.h>
#include <jailhouse/ivshmem.h>
#include <jailhouse/mmio.h>
#include <jailhouse/paging.h>
#include <jailhouse/pci.h>
#include <jailhouse/printk.h>
#include <jailhouse/sizes.h>
#include <jailhouse/string.h>
#include <jailhouse/unit.h>
#include <asm/bitops.h>
#include <asm/spinlock.h>
#include <jailhouse/cell-config.h>

#include "arm-smmu-regs.h"

#define ARM_32_LPAE_TCR_EAE		(1 << 31)
#define ARM_64_LPAE_S2_TCR_RES1		(1 << 31)

#define ARM_LPAE_TCR_EPD1		(1 << 23)

#define ARM_LPAE_TCR_TG0_4K		(0 << 14)
#define ARM_LPAE_TCR_TG0_64K		(1 << 14)
#define ARM_LPAE_TCR_TG0_16K		(2 << 14)

#define ARM_LPAE_TCR_SH0_SHIFT		12
#define ARM_LPAE_TCR_SH0_MASK		0x3
#define ARM_LPAE_TCR_SH_NS		0
#define ARM_LPAE_TCR_SH_OS		2
#define ARM_LPAE_TCR_SH_IS		3

#define ARM_LPAE_TCR_ORGN0_SHIFT	10
#define ARM_LPAE_TCR_IRGN0_SHIFT	8
#define ARM_LPAE_TCR_RGN_MASK		0x3
#define ARM_LPAE_TCR_RGN_NC		0
#define ARM_LPAE_TCR_RGN_WBWA		1
#define ARM_LPAE_TCR_RGN_WT		2
#define ARM_LPAE_TCR_RGN_WB		3

#define ARM_LPAE_TCR_SL0_SHIFT		6
#define ARM_LPAE_TCR_SL0_MASK		0x3
#define ARM_LPAE_TCR_SL0_LVL_2		0
#define ARM_LPAE_TCR_SL0_LVL_1		1

#define ARM_LPAE_TCR_T0SZ_SHIFT		0
#define ARM_LPAE_TCR_SZ_MASK		0xf

#define ARM_LPAE_TCR_PS_SHIFT		16
#define ARM_LPAE_TCR_PS_MASK		0x7

#define ARM_LPAE_TCR_IPS_SHIFT		32
#define ARM_LPAE_TCR_IPS_MASK		0x7

#define ARM_LPAE_TCR_PS_32_BIT		0x0ULL
#define ARM_LPAE_TCR_PS_36_BIT		0x1ULL
#define ARM_LPAE_TCR_PS_40_BIT		0x2ULL
#define ARM_LPAE_TCR_PS_42_BIT		0x3ULL
#define ARM_LPAE_TCR_PS_44_BIT		0x4ULL
#define ARM_LPAE_TCR_PS_48_BIT		0x5ULL
#define ARM_LPAE_TCR_PS_52_BIT		0x6ULL
#define ARM_MMU500_ACTLR_CPRE		(1 << 1)

#define ARM_MMU500_ACR_CACHE_LOCK	(1 << 26)
#define ARM_MMU500_ACR_S2CRB_TLBEN	(1 << 10)
#define ARM_MMU500_ACR_SMTNMB_TLBEN	(1 << 8)

#define TLB_LOOP_TIMEOUT		1000000	/* 1s! */
#define TLB_SPIN_COUNT			10

/* Maximum number of context banks per SMMU */
#define ARM_SMMU_MAX_CBS		128

/* SMMU global address space */
#define ARM_SMMU_GR0(smmu)		((smmu)->base)
#define ARM_SMMU_GR1(smmu)		((smmu)->base + (1 << (smmu)->pgshift))

/*
 * SMMU global address space with conditional offset to access secure
 * aliases of non-secure registers (e.g. nsCR0: 0x400, nsGFSR: 0x448,
 * nsGFSYNR0: 0x450)
 */
#define ARM_SMMU_GR0_NS(smmu)						\
	((smmu)->base +							\
		((smmu->options & ARM_SMMU_OPT_SECURE_CFG_ACCESS)	\
			? 0x400 : 0))

/* Translation context bank */
#define ARM_SMMU_CB(smmu, n)	((smmu)->cb_base + ((n) << (smmu)->pgshift))

#define MSI_IOVA_BASE			0x8000000
#define MSI_IOVA_LENGTH			0x100000

struct arm_smmu_s2cr {
	enum arm_smmu_s2cr_type		type;
	enum arm_smmu_s2cr_privcfg	privcfg;
	u8				cbndx;
};

struct arm_smmu_smr {
	u16				mask;
	u16				id;
	bool				valid;
};

struct arm_smmu_cb {
	u64				ttbr[2];
	u32				tcr[2];
	u32				mair[2];
	struct arm_smmu_cfg		*cfg;
};

struct arm_smmu_device {
	void	*base;
	void	*cb_base;
	u32	num_masters;
	unsigned long			pgshift;

#define ARM_SMMU_FEAT_COHERENT_WALK	(1 << 0)
#define ARM_SMMU_FEAT_STREAM_MATCH	(1 << 1)
#define ARM_SMMU_FEAT_TRANS_S1		(1 << 2)
#define ARM_SMMU_FEAT_TRANS_S2		(1 << 3)
#define ARM_SMMU_FEAT_TRANS_NESTED	(1 << 4)
#define ARM_SMMU_FEAT_TRANS_OPS		(1 << 5)
#define ARM_SMMU_FEAT_VMID16		(1 << 6)
#define ARM_SMMU_FEAT_FMT_AARCH64_4K	(1 << 7)
#define ARM_SMMU_FEAT_FMT_AARCH64_16K	(1 << 8)
#define ARM_SMMU_FEAT_FMT_AARCH64_64K	(1 << 9)
#define ARM_SMMU_FEAT_FMT_AARCH32_L	(1 << 10)
#define ARM_SMMU_FEAT_FMT_AARCH32_S	(1 << 11)
#define ARM_SMMU_FEAT_EXIDS		(1 << 12)
	u32				features;

#define ARM_SMMU_OPT_SECURE_CFG_ACCESS (1 << 0)
	u32				options;
	enum arm_smmu_arch_version	version;
	enum arm_smmu_implementation	model;

	u32				num_context_banks;
	u32				num_s2_context_banks;
	struct arm_smmu_cb		*cbs;

	u32				num_mapping_groups;
	u16				streamid_mask;
	u16				smr_mask_mask;
	struct arm_smmu_smr		*smrs;
	struct arm_smmu_s2cr		*s2crs;
	struct arm_smmu_cfg		*cfgs;

	unsigned long			va_size;
	unsigned long			ipa_size;
	unsigned long			pa_size;
	unsigned long			pgsize_bitmap;

	u32				num_global_irqs;
	u32				num_context_irqs;
	unsigned int			*irqs;

	spinlock_t			global_sync_lock;
};

enum arm_smmu_context_fmt {
	ARM_SMMU_CTX_FMT_NONE,
	ARM_SMMU_CTX_FMT_AARCH64,
	ARM_SMMU_CTX_FMT_AARCH32_L,
	ARM_SMMU_CTX_FMT_AARCH32_S,
};

struct arm_smmu_cfg {
	u8				cbndx;
	u8				irptndx;
	union {
		u16			asid;
		u16			vmid;
	};
	u32				cbar;
	enum arm_smmu_context_fmt	fmt;
};
#define INVALID_IRPTNDX			0xff

enum arm_smmu_domain_stage {
	ARM_SMMU_DOMAIN_S1 = 0,
	ARM_SMMU_DOMAIN_S2,
	ARM_SMMU_DOMAIN_NESTED,
	ARM_SMMU_DOMAIN_BYPASS,
};

#define s2cr_init_val (struct arm_smmu_s2cr){	\
	.type = S2CR_TYPE_BYPASS,		\
}


static struct arm_smmu_device smmu_device;
static unsigned long pgsize_bitmap = -1;
static u16 arm_sid_mask;

static void arm_smmu_write_smr(struct arm_smmu_device *smmu, int idx)
{
	struct arm_smmu_smr *smr = smmu->smrs + idx;
	u32 reg = smr->id << SMR_ID_SHIFT | smr->mask << SMR_MASK_SHIFT;

	if (!(smmu->features & ARM_SMMU_FEAT_EXIDS) && smr->valid)
		reg |= SMR_VALID;
	mmio_write32(ARM_SMMU_GR0(smmu) + ARM_SMMU_GR0_SMR(idx), reg);
}

static void arm_smmu_write_s2cr(struct arm_smmu_device *smmu, int idx)
{
	struct arm_smmu_s2cr *s2cr = smmu->s2crs + idx;
	u32 reg = (s2cr->type & S2CR_TYPE_MASK) << S2CR_TYPE_SHIFT |
		  (s2cr->cbndx & S2CR_CBNDX_MASK) << S2CR_CBNDX_SHIFT |
		  (s2cr->privcfg & S2CR_PRIVCFG_MASK) << S2CR_PRIVCFG_SHIFT;

	if (smmu->features & ARM_SMMU_FEAT_EXIDS && smmu->smrs &&
	    smmu->smrs[idx].valid)
		reg |= S2CR_EXIDVALID;
	mmio_write32(ARM_SMMU_GR0(smmu) + ARM_SMMU_GR0_S2CR(idx), reg);
}

static void arm_smmu_write_sme(struct arm_smmu_device *smmu, int idx)
{
	if (smmu->smrs)
		arm_smmu_write_smr(smmu, idx);

	arm_smmu_write_s2cr(smmu, idx);
}

/* Wait for any pending TLB invalidations to complete */
static void __arm_smmu_tlb_sync(struct arm_smmu_device *smmu,
				void *sync, void *status)
{
	unsigned int spin_cnt, delay;

	mmio_write32(sync, 0);
	for (delay = 1; delay < TLB_LOOP_TIMEOUT; delay *= 2) {
		for (spin_cnt = TLB_SPIN_COUNT; spin_cnt > 0; spin_cnt--) {
			if (!(mmio_read32(status) & sTLBGSTATUS_GSACTIVE))
				return;
			cpu_relax();
		}
	}
	printk("TLB sync timed out -- SMMU may be deadlocked\n");
}

static void arm_smmu_tlb_sync_global(struct arm_smmu_device *smmu)
{
	void *base = ARM_SMMU_GR0(smmu);

	spin_lock(&smmu->global_sync_lock);
	__arm_smmu_tlb_sync(smmu, base + ARM_SMMU_GR0_sTLBGSYNC,
			    base + ARM_SMMU_GR0_sTLBGSTATUS);
	spin_unlock(&smmu->global_sync_lock);
}

#define ARM_SMMU_CB_VMID(cfg)		((cfg)->cbndx + 1)
static void arm_smmu_init_context_bank(struct arm_smmu_device *smmu,
				       struct arm_smmu_cfg *cfg,
				       struct cell *cell)
{
	struct arm_smmu_cb *cb = &smmu->cbs[cfg->cbndx];
	struct paging_structures *pg_structs;
	unsigned long cell_table;
	u32 reg;

	cb->cfg = cfg;

	/* VTCR */
	reg = ARM_64_LPAE_S2_TCR_RES1 |
	     (ARM_LPAE_TCR_SH_IS << ARM_LPAE_TCR_SH0_SHIFT) |
	     (ARM_LPAE_TCR_RGN_WBWA << ARM_LPAE_TCR_IRGN0_SHIFT) |
	     (ARM_LPAE_TCR_RGN_WBWA << ARM_LPAE_TCR_ORGN0_SHIFT);

	reg |= (ARM_LPAE_TCR_SL0_LVL_1 << ARM_LPAE_TCR_SL0_SHIFT);

	switch (PAGE_SIZE) {
	case SZ_4K:
		reg |= ARM_LPAE_TCR_TG0_4K;
		break;
	case SZ_64K:
		reg |= ARM_LPAE_TCR_TG0_64K;
		break;
	}

	switch (smmu->pa_size) {
	case 32:
		reg |= (ARM_LPAE_TCR_PS_32_BIT << ARM_LPAE_TCR_PS_SHIFT);
		break;
	case 36:
		reg |= (ARM_LPAE_TCR_PS_36_BIT << ARM_LPAE_TCR_PS_SHIFT);
		break;
	case 40:
		reg |= (ARM_LPAE_TCR_PS_40_BIT << ARM_LPAE_TCR_PS_SHIFT);
		break;
	case 42:
		reg |= (ARM_LPAE_TCR_PS_42_BIT << ARM_LPAE_TCR_PS_SHIFT);
		break;
	case 44:
		reg |= (ARM_LPAE_TCR_PS_44_BIT << ARM_LPAE_TCR_PS_SHIFT);
		break;
	case 48:
		reg |= (ARM_LPAE_TCR_PS_48_BIT << ARM_LPAE_TCR_PS_SHIFT);
		break;
	case 52:
		reg |= (ARM_LPAE_TCR_PS_52_BIT << ARM_LPAE_TCR_PS_SHIFT);
		break;
	default:
		printk("Not supported\n");
		break;
		/* TODO */
		//goto out_free_data;
	}

	reg |= (64ULL - smmu->ipa_size) << ARM_LPAE_TCR_T0SZ_SHIFT;

	cb->tcr[0] = reg;

	pg_structs = &cell->arch.mm;
	cell_table = paging_hvirt2phys(pg_structs->root_table);
	u64 vttbr = 0;

	vttbr |= (u64)cell->config->id << VTTBR_VMID_SHIFT;
	vttbr |= (u64)(cell_table & TTBR_MASK);
	cb->ttbr[0] = vttbr;
}

static void arm_smmu_write_context_bank(struct arm_smmu_device *smmu, int idx)
{
	u32 reg;
	struct arm_smmu_cb *cb = &smmu->cbs[idx];
	struct arm_smmu_cfg *cfg = cb->cfg;
	void *cb_base, *gr1_base;

	cb_base = ARM_SMMU_CB(smmu, idx);

	/* Unassigned context banks only need disabling */
	if (!cfg) {
		mmio_write32(cb_base + ARM_SMMU_CB_SCTLR, 0);
		return;
	}

	gr1_base = ARM_SMMU_GR1(smmu);

	/* CBA2R */
	if (smmu->version > ARM_SMMU_V1) {
		if (cfg->fmt == ARM_SMMU_CTX_FMT_AARCH64)
			reg = CBA2R_RW64_64BIT;
		else
			reg = CBA2R_RW64_32BIT;
		/* 16-bit VMIDs live in CBA2R */
		if (smmu->features & ARM_SMMU_FEAT_VMID16)
			reg |= cfg->vmid << CBA2R_VMID_SHIFT;

		mmio_write32(gr1_base + ARM_SMMU_GR1_CBA2R(idx), reg);
	}

	/* CBAR */
	reg = cfg->cbar;
	if (smmu->version < ARM_SMMU_V2)
		reg |= cfg->irptndx << CBAR_IRPTNDX_SHIFT;

	/*
	 * Use the weakest shareability/memory types, so they are
	 * overridden by the ttbcr/pte.
	 */
	if (!(smmu->features & ARM_SMMU_FEAT_VMID16)) {
		/* 8-bit VMIDs live in CBAR */
		reg |= cfg->vmid << CBAR_VMID_SHIFT;
	}
	mmio_write32(gr1_base + ARM_SMMU_GR1_CBAR(idx), reg);

	/*
	 * TTBCR
	 * We must write this before the TTBRs, since it determines the
	 * access behaviour of some fields (in particular, ASID[15:8]).
	 */
	mmio_write32(cb_base + ARM_SMMU_CB_TTBCR, cb->tcr[0]);

	/* TTBRs */
	if (cfg->fmt == ARM_SMMU_CTX_FMT_AARCH32_S) {
		mmio_write32(cb_base + ARM_SMMU_CB_CONTEXTIDR, cfg->asid);
		mmio_write32(cb_base + ARM_SMMU_CB_TTBR0, cb->ttbr[0]);
		mmio_write32(cb_base + ARM_SMMU_CB_TTBR1, cb->ttbr[1]);
	} else {
		mmio_write64(cb_base + ARM_SMMU_CB_TTBR0, cb->ttbr[0]);
	}

	/* SCTLR */
	reg = SCTLR_CFCFG | SCTLR_CFIE | SCTLR_CFRE | SCTLR_AFE | SCTLR_TRE |
		SCTLR_M;

	mmio_write32(cb_base + ARM_SMMU_CB_SCTLR, reg);
}

static void arm_smmu_device_reset(struct arm_smmu_device *smmu)
{
	void *gr0_base = ARM_SMMU_GR0(smmu);
	int i;
	u32 reg, major;

	/* clear global FSR */
	reg = mmio_read32(ARM_SMMU_GR0_NS(smmu) + ARM_SMMU_GR0_sGFSR);
	mmio_write32(ARM_SMMU_GR0_NS(smmu) + ARM_SMMU_GR0_sGFSR, reg);

	/*
	 * Reset stream mapping groups: Initial values mark all SMRn as
	 * invalid and all S2CRn as bypass unless overridden.
	 */
	for (i = 0; i < smmu->num_mapping_groups; ++i)
		arm_smmu_write_sme(smmu, i);

	if (smmu->model == ARM_MMU500) {
		/*
		 * Before clearing ARM_MMU500_ACTLR_CPRE, need to
		 * clear CACHE_LOCK bit of ACR first. And, CACHE_LOCK
		 * bit is only present in MMU-500r2 onwards.
		 */
		reg = mmio_read32(gr0_base + ARM_SMMU_GR0_ID7);
		major = (reg >> ID7_MAJOR_SHIFT) & ID7_MAJOR_MASK;
		reg = mmio_read32(gr0_base + ARM_SMMU_GR0_sACR);
		if (major >= 2)
			reg &= ~ARM_MMU500_ACR_CACHE_LOCK;
		/*
		 * Allow unmatched Stream IDs to allocate bypass
		 * TLB entries for reduced latency.
		 */
		reg |= ARM_MMU500_ACR_SMTNMB_TLBEN | ARM_MMU500_ACR_S2CRB_TLBEN;
		mmio_write32(gr0_base + ARM_SMMU_GR0_sACR, reg);
	}

	/* Make sure all context banks are disabled and clear CB_FSR  */
	for (i = 0; i < smmu->num_context_banks; ++i) {
		void *cb_base = ARM_SMMU_CB(smmu, i);

		arm_smmu_write_context_bank(smmu, i);
		mmio_write32(cb_base + ARM_SMMU_CB_FSR, FSR_FAULT);
		/*
		 * Disable MMU-500's not-particularly-beneficial next-page
		 * prefetcher for the sake of errata #841119 and #826419.
		 */
		if (smmu->model == ARM_MMU500) {
			reg = mmio_read32(cb_base + ARM_SMMU_CB_ACTLR);
			reg &= ~ARM_MMU500_ACTLR_CPRE;
			mmio_write32(cb_base + ARM_SMMU_CB_ACTLR, reg);
		}
	}

	/* Invalidate the TLB, just in case */
	mmio_write32(gr0_base + ARM_SMMU_GR0_TLBIALLH, 0);
	mmio_write32(gr0_base + ARM_SMMU_GR0_TLBIALLNSNH, 0);

	reg = mmio_read32(ARM_SMMU_GR0_NS(smmu) + ARM_SMMU_GR0_sCR0);

	/* Enable fault reporting */
	reg |= (sCR0_GFRE | sCR0_GFIE | sCR0_GCFGFRE | sCR0_GCFGFIE);

	/* Disable TLB broadcasting. */
	reg |= (sCR0_VMIDPNE | sCR0_PTM);

	/* Enable client access, handling unmatched streams as appropriate */
	reg &= ~sCR0_CLIENTPD;
	reg &= ~sCR0_USFCFG;

	/* Disable forced broadcasting */
	reg &= ~sCR0_FB;

	/* Don't upgrade barriers */
	reg &= ~(sCR0_BSU_MASK << sCR0_BSU_SHIFT);

	if (smmu->features & ARM_SMMU_FEAT_VMID16)
		reg |= sCR0_VMID16EN;

	if (smmu->features & ARM_SMMU_FEAT_EXIDS)
		reg |= sCR0_EXIDENABLE;

	/* Push the button */
	arm_smmu_tlb_sync_global(smmu);
	mmio_write32(ARM_SMMU_GR0_NS(smmu) + ARM_SMMU_GR0_sCR0, reg);
}

static int arm_smmu_id_size_to_bits(int size)
{
	switch (size) {
	case 0:
		return 32;
	case 1:
		return 36;
	case 2:
		return 40;
	case 3:
		return 42;
	case 4:
		return 44;
	case 5:
	default:
		return 48;
	}
}

static int arm_smmu_device_cfg_probe(struct arm_smmu_device *smmu)
{
	unsigned long size;
	void *gr0_base = ARM_SMMU_GR0(smmu);
	u32 id;
	bool cttw_reg, cttw_fw = smmu->features & ARM_SMMU_FEAT_COHERENT_WALK;
	int i;

	printk("probing hardware configuration...\n");
	printk("SMMUv%d with:\n", smmu->version == ARM_SMMU_V2 ? 2 : 1);

	/* ID0 */
	id = mmio_read32(gr0_base + ARM_SMMU_GR0_ID0);

	/* Restrict stage 2 */
	id &= ~(ID0_S1TS | ID0_NTS);

	if (id & ID0_S2TS) {
		smmu->features |= ARM_SMMU_FEAT_TRANS_S2;
		printk("\tstage 2 translation\n");
	}

	if (!(smmu->features &
		(ARM_SMMU_FEAT_TRANS_S1 | ARM_SMMU_FEAT_TRANS_S2))) {
		printk("\tno translation support!\n");
		return -ENODEV;
	}

	/*
	 * In order for DMA API calls to work properly, we must defer to what
	 * the FW says about coherency, regardless of what the hardware claims.
	 * Fortunately, this also opens up a workaround for systems where the
	 * ID register value has ended up configured incorrectly.
	 */
	cttw_reg = !!(id & ID0_CTTW);
	if (cttw_fw || cttw_reg)
		printk("\t%scoherent table walk\n", cttw_fw ? "" : "non-");
	if (cttw_fw != cttw_reg)
		printk("\t(IDR0.CTTW overridden by FW configuration)\n");

	/* Max. number of entries we have for stream matching/indexing */
	if (smmu->version == ARM_SMMU_V2 && id & ID0_EXIDS) {
		smmu->features |= ARM_SMMU_FEAT_EXIDS;
		size = 1 << 16;
	} else {
		size = 1 << ((id >> ID0_NUMSIDB_SHIFT) & ID0_NUMSIDB_MASK);
	}
	smmu->streamid_mask = size - 1;

	if (id & ID0_SMS) {
		smmu->features |= ARM_SMMU_FEAT_STREAM_MATCH;
		size = (id >> ID0_NUMSMRG_SHIFT) & ID0_NUMSMRG_MASK;
		if (size == 0) {
			printk("stream-matching supported, but no SMRs present!\n");
			return -ENODEV;
		}

		/* Zero-initialised to mark as invalid */
		smmu->smrs = page_alloc(&mem_pool,
					PAGES(size * sizeof(*smmu->smrs)));
		if (!smmu->smrs)
			return -ENOMEM;
		memset(smmu->smrs, 0, PAGES(size * sizeof(*smmu->smrs)));

		printk("\tstream matching with %lu register groups", size);
	}
	/* s2cr->type == 0 means translation, so initialise explicitly */
	smmu->s2crs = page_alloc(&mem_pool, PAGES(size * (sizeof(*smmu->s2crs) + sizeof(*smmu->cfgs))));
	if (!smmu->s2crs) {
		page_free(&mem_pool, smmu->smrs,
			  PAGES(size * sizeof(*smmu->smrs)));
		return -ENOMEM;
	}

	smmu->cfgs = (struct arm_smmu_cfg *)(smmu->s2crs + size);

	/* Configure to bypass */
	for (i = 0; i < size; i++)
		smmu->s2crs[i] = s2cr_init_val;

	smmu->num_mapping_groups = size;

	if (smmu->version < ARM_SMMU_V2 || !(id & ID0_PTFS_NO_AARCH32)) {
		smmu->features |= ARM_SMMU_FEAT_FMT_AARCH32_L;
		if (!(id & ID0_PTFS_NO_AARCH32S))
			smmu->features |= ARM_SMMU_FEAT_FMT_AARCH32_S;
	}

	/* ID1 */
	id = mmio_read32(gr0_base + ARM_SMMU_GR0_ID1);
	smmu->pgshift = (id & ID1_PAGESIZE) ? 16 : 12;

	/* Check for size mismatch of SMMU address space from mapped region */
	size = 1 << (((id >> ID1_NUMPAGENDXB_SHIFT) & ID1_NUMPAGENDXB_MASK) + 1);
	size <<= smmu->pgshift;
	if (smmu->cb_base != gr0_base + size)
		printk("SMMU address space size (0x%lx) differs from mapped region size (0x%tx)!\n",
		       size * 2, (smmu->cb_base - gr0_base) * 2);

	smmu->num_s2_context_banks = (id >> ID1_NUMS2CB_SHIFT) & ID1_NUMS2CB_MASK;
	smmu->num_context_banks = (id >> ID1_NUMCB_SHIFT) & ID1_NUMCB_MASK;
	if (smmu->num_s2_context_banks > smmu->num_context_banks) {
		printk("impossible number of S2 context banks!\n");
		return -ENODEV;
	}
	/* TODO Check More */
	smmu->num_context_irqs = smmu->num_context_banks;

	printk("\t%u context banks (%u stage-2 only)\n",
	       smmu->num_context_banks, smmu->num_s2_context_banks);

	smmu->cbs = page_alloc(&mem_pool, PAGES(smmu->num_context_banks * sizeof(*smmu->cbs)));
	if (!smmu->cbs) {
		/* TODO: Free smrs s2cr */
		return -ENOMEM;
	}

	/* ID2 */
	id = mmio_read32(gr0_base + ARM_SMMU_GR0_ID2);
	size = arm_smmu_id_size_to_bits((id >> ID2_IAS_SHIFT) & ID2_IAS_MASK);
	/* Reuse cpu table */
	smmu->ipa_size = MIN(size, get_cpu_parange());

	/* The output mask is also applied for bypass */
	size = arm_smmu_id_size_to_bits((id >> ID2_OAS_SHIFT) & ID2_OAS_MASK);
	smmu->pa_size = size;

	if (id & ID2_VMID16)
		smmu->features |= ARM_SMMU_FEAT_VMID16;

	/*
	 * What the page table walker can address actually depends on which
	 * descriptor format is in use, but since a) we don't know that yet,
	 * and b) it can vary per context bank, this will have to do...
	 * TODO: DMA?
	 */

	if (smmu->version < ARM_SMMU_V2) {
		smmu->va_size = smmu->ipa_size;
		if (smmu->version == ARM_SMMU_V1_64K)
			smmu->features |= ARM_SMMU_FEAT_FMT_AARCH64_64K;
	} else {
		size = (id >> ID2_UBS_SHIFT) & ID2_UBS_MASK;
		smmu->va_size = arm_smmu_id_size_to_bits(size);
		if (id & ID2_PTFS_4K)
			smmu->features |= ARM_SMMU_FEAT_FMT_AARCH64_4K;
		if (id & ID2_PTFS_16K)
			smmu->features |= ARM_SMMU_FEAT_FMT_AARCH64_16K;
		if (id & ID2_PTFS_64K)
			smmu->features |= ARM_SMMU_FEAT_FMT_AARCH64_64K;
	}

	/* Now we've corralled the various formats, what'll it do? */
	if (smmu->features & ARM_SMMU_FEAT_FMT_AARCH32_S)
		smmu->pgsize_bitmap |= SZ_4K | SZ_64K | SZ_1M | SZ_16M;
	if (smmu->features &
	    (ARM_SMMU_FEAT_FMT_AARCH32_L | ARM_SMMU_FEAT_FMT_AARCH64_4K))
		smmu->pgsize_bitmap |= SZ_4K | SZ_2M | SZ_1G;
	if (smmu->features & ARM_SMMU_FEAT_FMT_AARCH64_16K)
		smmu->pgsize_bitmap |= SZ_16K | SZ_32M;
	if (smmu->features & ARM_SMMU_FEAT_FMT_AARCH64_64K)
		smmu->pgsize_bitmap |= SZ_64K | SZ_512M;

	if (pgsize_bitmap == -1UL)
		pgsize_bitmap = smmu->pgsize_bitmap;
	else
		pgsize_bitmap |= smmu->pgsize_bitmap;
	printk("\tSupported page sizes: 0x%08lx\n", smmu->pgsize_bitmap);


	if (smmu->features & ARM_SMMU_FEAT_TRANS_S1)
		printk("\tStage-1: %lu-bit VA -> %lu-bit IPA\n",
		       smmu->va_size, smmu->ipa_size);

	if (smmu->features & ARM_SMMU_FEAT_TRANS_S2)
		printk("\tStage-2: %lu-bit IPA -> %lu-bit PA\n",
		       smmu->ipa_size, smmu->pa_size);

	return 0;
}

static void arm_smmu_test_smr_masks(struct arm_smmu_device *smmu)
{
	void *gr0_base = ARM_SMMU_GR0(smmu);
	u32 smr;

	if (!smmu->smrs)
		return;

	/*
	 * SMR.ID bits may not be preserved if the corresponding MASK
	 * bits are set, so check each one separately. We can reject
	 * masters later if they try to claim IDs outside these masks.
	 */
	smr = smmu->streamid_mask << SMR_ID_SHIFT;
	mmio_write32(gr0_base + ARM_SMMU_GR0_SMR(0), smr);
	smr = mmio_read32(gr0_base + ARM_SMMU_GR0_SMR(0));
	smmu->streamid_mask = smr >> SMR_ID_SHIFT;

	smr = smmu->streamid_mask << SMR_MASK_SHIFT;
	mmio_write32(gr0_base + ARM_SMMU_GR0_SMR(0), smr);
	smr = mmio_read32(gr0_base + ARM_SMMU_GR0_SMR(0));
	smmu->smr_mask_mask = smr >> SMR_MASK_SHIFT;
}

static int arm_smmu_find_sme(u16 id, u16 mask)
{
	struct arm_smmu_device *smmu = &smmu_device;
	struct arm_smmu_smr *smrs = smmu->smrs;
	int i, free_idx = -EINVAL;

	/* Stream indexing is blissfully easy */
	if (!smrs)
		return id;

	/* Validating SMRs is... less so */
	for (i = 0; i < smmu->num_mapping_groups; ++i) {
		if (!smrs[i].valid) {
			/*
			 * Note the first free entry we come across, which
			 * we'll claim in the end if nothing else matches.
			 */
			if (free_idx < 0)
				free_idx = i;
			continue;
		}
		/*
		 * If the new entry is _entirely_ matched by an existing entry,
		 * then reuse that, with the guarantee that there also cannot
		 * be any subsequent conflicting entries. In normal use we'd
		 * expect simply identical entries for this case, but there's
		 * no harm in accommodating the generalisation.
		 */
		if ((mask & smrs[i].mask) == mask &&
		    !((id ^ smrs[i].id) & ~smrs[i].mask)) {
			return i;
		}
		/*
		 * If the new entry has any other overlap with an existing one,
		 * though, then there always exists at least one stream ID
		 * which would cause a conflict, and we can't allow that risk.
		 */
		if (!((id ^ smrs[i].id) & ~(smrs[i].mask | mask)))
			return -EINVAL;
	}

	return free_idx;
}

static bool arm_smmu_free_sme(struct arm_smmu_device *smmu, int idx)
{
	smmu->s2crs[idx] = s2cr_init_val;
	if (smmu->smrs) {
		smmu->smrs[idx].id = 0;
		smmu->smrs[idx].mask = 0;
		smmu->smrs[idx].valid = false;
	}

	return true;
}

#define for_each_smmu_sid(sid, config, counter)				\
	for ((sid) = jailhouse_cell_stream_ids(config), (counter) = 0;	\
	     (counter) < (config)->num_stream_ids;			\
	     (sid)++, (counter)++)

static int arm_smmu_cell_init(struct cell *cell)
{
	struct arm_smmu_device *smmu = &smmu_device;
	enum arm_smmu_s2cr_type type = S2CR_TYPE_TRANS;
	struct arm_smmu_s2cr *s2cr = smmu->s2crs;
	struct arm_smmu_cfg *cfg = &smmu->cfgs[cell->config->id];
	struct arm_smmu_smr *smr;
	const __u32 *sid;
	unsigned int n;
	int ret, idx;

	/* If no sids, ignore */
	if (!cell->config->num_stream_ids)
		return 0;

	if (smmu->features & (ARM_SMMU_FEAT_FMT_AARCH64_64K |
			      ARM_SMMU_FEAT_FMT_AARCH64_16K |
			      ARM_SMMU_FEAT_FMT_AARCH64_4K))
		cfg->fmt = ARM_SMMU_CTX_FMT_AARCH64;

	cfg->cbar = CBAR_TYPE_S2_TRANS;

	/* We use cell->config->id here, one cell use one context */
	cfg->cbndx = cell->config->id;

	if (smmu->version < ARM_SMMU_V2) {
		/* TODO */
	} else {
		cfg->irptndx = cfg->cbndx;
	}

	cfg->vmid = cfg->cbndx + 1;

	arm_smmu_init_context_bank(smmu, cfg, cell);
	arm_smmu_write_context_bank(smmu, cfg->cbndx);

	smr = smmu->smrs;

	for_each_smmu_sid(sid, cell->config, n) {
		ret = arm_smmu_find_sme(*sid, arm_sid_mask);
		if (ret < 0)
			printk("arm_smmu_find_sme error %d\n", ret);
		idx = ret;

		if (type == s2cr[idx].type && cfg->cbndx == s2cr[idx].cbndx)
			printk("%s error\n", __func__);

		s2cr[idx].type = type;
		s2cr[idx].privcfg = S2CR_PRIVCFG_DEFAULT;
		s2cr[idx].cbndx = cfg->cbndx;

		arm_smmu_write_s2cr(smmu, idx);


		smr[idx].id = *sid;
		smr[idx].mask = arm_sid_mask;
		smr[idx].valid = true;

		arm_smmu_write_smr(smmu, idx);
	}

	printk("Found %d masters\n", n);

	return 0;
}

static void arm_smmu_cell_exit(struct cell *cell)
{
	const __u32 *sid;
	struct arm_smmu_device *smmu = &smmu_device;
	unsigned int n;
	int ret, idx;
	int cbndx = cell->config->id;

	if (!cell->config->num_stream_ids)
		return;

	/* If no sids, ignore */
	if (cell->config->num_stream_ids) {
		for_each_smmu_sid(sid, cell->config, n) {
			ret = arm_smmu_find_sme(*sid, arm_sid_mask);
			if (ret < 0)
				printk("arm_smmu_find_sme error %d\n", ret);
			idx = ret;

			if (arm_smmu_free_sme(smmu, idx)) {
				arm_smmu_write_sme(smmu, idx);
			}

			smmu->cbs[cbndx].cfg = NULL;
			arm_smmu_write_context_bank(smmu, cbndx);
		}
	}
}

static int arm_smmu_init(void)
{
	int err;
	__u64 phys_addr;
	__u32 size;
	struct arm_smmu_device *smmu = &smmu_device;

	smmu->features &= ~ARM_SMMU_FEAT_COHERENT_WALK;

	phys_addr = system_config->platform_info.arm.iommu_units[0].base;
	if (!phys_addr) {
		printk("No SMMU\n");
		return 0;
	}

	size = system_config->platform_info.arm.iommu_units[0].size;

	smmu->version =
		system_config->platform_info.arm.iommu_units[0].arm_smmu_arch;
	smmu->model =
		system_config->platform_info.arm.iommu_units[0].arm_smmu_impl;
	arm_sid_mask =
		system_config->platform_info.arm.iommu_units[0].arm_sid_mask;
	smmu->base = paging_map_device(phys_addr, size);
	if (!smmu->base)
		return -ENOMEM;

	smmu->cb_base = smmu->base + size / 2;

	err = arm_smmu_device_cfg_probe(smmu);
	if (err)
		return err;

	if (smmu->version == ARM_SMMU_V2 &&
	    smmu->num_context_banks != smmu->num_context_irqs) {
		printk("found only %d context interrupt(s) but %d required\n",
		       smmu->num_context_irqs, smmu->num_context_banks);
		/* TODO: page free smr s2cr cbs */
		return -ENODEV;
	}

	/* TODO: request irq */

	arm_smmu_device_reset(smmu);
	arm_smmu_test_smr_masks(smmu);

	/*
	 * For ACPI and generic DT bindings, an SMMU will be probed before
	 * any device which might need it, so we want the bus ops in place
	 * ready to handle default domain setup as soon as any SMMU exists.
	 */
	/* TODO: How handle PCI iommu? */

	return arm_smmu_cell_init(&root_cell);
}

DEFINE_UNIT_MMIO_COUNT_REGIONS_STUB(arm_smmu);
DEFINE_UNIT_SHUTDOWN_STUB(arm_smmu);
DEFINE_UNIT(arm_smmu, "ARM SMMU");
