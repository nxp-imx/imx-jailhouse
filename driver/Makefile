#
# Jailhouse, a Linux-based partitioning hypervisor
#
# Copyright (c) Siemens AG, 2013-2015
#
# Authors:
#  Jan Kiszka <jan.kiszka@siemens.com>
#  Benjamin Block <bebl@mageta.org>
#
# This work is licensed under the terms of the GNU GPL, version 2.  See
# the COPYING file in the top-level directory.
#

obj-m := jailhouse.o

ccflags-y := -I$(src)/../hypervisor/arch/$(SRCARCH)/include \
	     -I$(src)/../hypervisor/include \
	     -I$(src)/../include/arch/$(SRCARCH) \
	     -I$(src)/../include

jailhouse-y := cell.o main.o sysfs.o
jailhouse-$(CONFIG_PCI) += pci.o
jailhouse-$(CONFIG_OF) += vpci_template.dtb.o

targets += vpci_template.dtb vpci_template.dtb.S

kernel_version = $(shell expr $(VERSION) \* 65536 + $(PATCHLEVEL) \* 256)
kernel_v_6_3 = $(shell expr 6 \* 65536 + 3 \* 256)
ifeq ($(shell expr $(kernel_version) \>= $(kernel_v_6_3)),1)
ifneq ($(and $(filter notintermediate, $(.FEATURES)),$(filter-out 4.4,$(MAKE_VERSION))),)
.NOTINTERMEDIATE: \
        $(obj)/vpci_template.dtb.S \
        $(obj)/vpci_template.dtb
else
.SECONDARY: \
        $(obj)/vpci_template.dtb.S \
        $(obj)/vpci_template.dtb
endif
else
.SECONDARY: \
        $(obj)/vpci_template.dtb.S \
        $(obj)/vpci_template.dtb
endif
