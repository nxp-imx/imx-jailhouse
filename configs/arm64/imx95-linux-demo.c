/*
 * i.MX95 Target - linux-demo
 *
 * Copyright 2023 NXP
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

/*
 * Boot the 2nd Linux:
 * jailhouse cell linux imx95-linux-demo.cell Image -d imx95-19x19-evk-inmate.dtb -c \
 * "clk_ignore_unused earlycon=lpuart32,mmio32,0x44380010,115200 \
 * root=/dev/mmcblk0p2 rootwait rw"
 *
 * create a file /etc/systemd/network/20-wired.network with following content in rootfs
 * for a static IP configuration:
* [Match]
* Name=eth0
* [Network]
* Address=192.168.1.12/24
*
* After inmate up, use ssh root@192.168.1.12 to login inmate
 *
 * Note: Current lpuart3 is used as of now, but this needs configure SM
 * to open access from AP side. so as of now, for inmate linux, use ssh login
 * inmate linux is fine.
 */

#include <jailhouse/types.h>
#include <jailhouse/cell-config.h>

struct {
	struct jailhouse_cell_desc cell;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[24];
	struct jailhouse_irqchip irqchips[2];
	struct jailhouse_pci_device pci_devices[4];
} __attribute__((packed)) config = {
	.cell = {
		.signature = JAILHOUSE_CELL_DESC_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.architecture = JAILHOUSE_ARM64,
		.name = "linux-inmate-demo",
		.flags = JAILHOUSE_CELL_PASSIVE_COMMREG,

		.cpu_set_size = sizeof(config.cpus),
		.num_memory_regions = ARRAY_SIZE(config.mem_regions),
		.num_irqchips = ARRAY_SIZE(config.irqchips),
		.num_pci_devices = ARRAY_SIZE(config.pci_devices),
		.vpci_irq_base = 256,
	},

	.cpus = {
		0x18,
	},

	.mem_regions = {
		/* IVSHMEM shared memory region (virtio-blk front) */
		{
			.phys_start = 0xff900000,
			.virt_start = 0xff900000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0xff901000,
			.virt_start = 0xff901000,
			.size = 0xdf000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		{ 0 },
		{ 0 },
		/* IVSHMEM shared memory region (virtio-con front) */
		{
			.phys_start = 0xff9e0000,
			.virt_start = 0xff9e0000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0xff9e1000,
			.virt_start = 0xff9e1000,
			.size = 0xf000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		{ 0 },
		{ 0 },
		/* IVHSMEM shared memory region for 00:00.0 (demo )*/
		{
			.phys_start = 0xff9f0000,
			.virt_start = 0xff9f0000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0xff9f1000,
			.virt_start = 0xff9f1000,
			.size = 0x9000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0xff9fa000,
			.virt_start = 0xff9fa000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0xff9fc000,
			.virt_start = 0xff9fc000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0xff9fe000,
			.virt_start = 0xff9fe000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		/* IVSHMEM shared memory regions for 00:01.0 (networking) */
		JAILHOUSE_SHMEM_NET_REGIONS(0xffa00000, 1),
		/* earlycon: lpuart1 */
		{
			.phys_start = 0x44380000,
			.virt_start = 0x44380000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* lpuart3 */
		{
			.phys_start = 0x42570000,
			.virt_start = 0x42570000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* usdhc1 */
		{
			.phys_start = 0x42850000,
			.virt_start = 0x42850000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* RAM: Top at 4GB Space */ {
			.phys_start = 0xffb00000,
			.virt_start = 0,
			.size = 0x10000, /* 64KB */
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_LOADABLE,
		},
		/* RAM */ {
			/*
			 * We could not use 0x80000000 which conflicts with
			 * COMM_REGION_BASE
			 */
			.phys_start = 0xf0000000,
			.virt_start = 0xf0000000,
			.size = 0xf600000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_DMA |
				JAILHOUSE_MEM_LOADABLE,
		},
		/* 7GB DDR RAM */ {
			.phys_start = 0x100000000,
			.virt_start = 0x100000000,
			.size = 0x1c0000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_LOADABLE,
		},
		/* communication region */ {
			.virt_start = 0x80000000,
			.size = 0x00001000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_COMM_REGION,
		},
	},

	.irqchips = {
		{
			/* lpuart3/usdhc1 */
			.address = 0x48000000,
			.pin_base = 32,
			.pin_bitmap = {
				0, 0, (1 << (86 + 32 - 32 - 64)) | (1 << (64 + 32 - 32 - 64)), 0
			},
		},
		{
			.address = 0x48000000,
			.pin_base = 288,
			.pin_bitmap = {
				0xf << (256 + 32 - 288)
			},
		},
	},

	.pci_devices = {
		{
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 2,
			.bdf = 2 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 0,
			.shmem_dev_id = 1,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VIRTIO_FRONT +
				VIRTIO_DEV_BLOCK,
		},
		{
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 2,
			.bdf = 3 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 4,
			.shmem_dev_id = 1,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VIRTIO_FRONT +
				VIRTIO_DEV_CONSOLE,
		},
		{ /* IVSHMEM 00:00.0 (demo) */
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 2,
			.bdf = 0 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 8,
			.shmem_dev_id = 2,
			.shmem_peers = 3,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_UNDEFINED,
		},
		{ /* IVSHMEM 00:01.0 (networking) */
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 2,
			.bdf = 1 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 13,
			.shmem_dev_id = 1,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VETH,
		},
	},
};
