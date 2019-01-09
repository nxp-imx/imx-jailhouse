/*
 * Jailhouse, a Linux-based partitioning hypervisor
 *
 * Copyright 2018 NXP
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

static inline int iommu_add_pci_device(struct cell *cell, struct pci_device *device) {return 0;};
static inline void iommu_remove_pci_device(struct pci_device *device) {};
static inline int iommu_map_memory_region(struct cell *cell, const struct jailhouse_memory *mem) {return 0;};
static inline int iommu_unmap_memory_region(struct cell *cell, const struct jailhouse_memory *mem) {return 0;};
static inline void iommu_config_commit(struct cell *cell_added_removed) {};
static inline void iommu_prepare_shutdown(void) {};
#endif
