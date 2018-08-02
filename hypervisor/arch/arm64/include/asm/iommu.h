/*
 * Jailhouse, a Linux-based partitioning hypervisor
 *
 * Copyright 2017 NXP
 *
 * Authors:
 *  Peng Fan <peng.fan@nxp.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#ifndef _JAILHOUSE_ASM_IOMMU_H
#define _JAILHOUSE_ASM_IOMMU_H

#include <jailhouse/entry.h>
#include <jailhouse/cell.h>
#include <jailhouse/cell-config.h>
#include <jailhouse/pci.h>
#include <jailhouse/types.h>
#include <asm/percpu.h>

int iommu_add_pci_device(struct cell *cell, struct pci_device *device);
void iommu_remove_pci_device(struct pci_device *device);
int iommu_map_memory_region(struct cell *cell, const struct jailhouse_memory *mem);
int iommu_unmap_memory_region(struct cell *cell, const struct jailhouse_memory *mem);
void iommu_config_commit(struct cell *cell_added_removed);
void iommu_prepare_shutdown(void);
#endif
