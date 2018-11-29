/*
 * ivshmem test program 
 *
 * Copyright (c) NXP 2018
 *
 * Authors:
 *  flynn <flynn.xu@nxp.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

enum ivshmem_registers {
	IntrMask = 0,
	IntrStatus = 4,
	IVPosition = 8,
	Doorbell = 12,
	IVLiveList = 16
};

int main(int argc, char ** argv)
{
	unsigned int * map_array;
	int i, fd, cnt, msg, cmd, dest;
	void * memptr;

	if (argc != 5) {
		printf("USAGE: %s <filename> <count> <cmd> <dest>\n", argv[0]);
		exit(-1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		printf("error on open %s\n", argv[1]);
		exit(-1);
	}

	cnt = atol(argv[2]);
	cmd = (unsigned short) atoi(argv[3]);
	dest = (unsigned short) atoi(argv[4]);

	memptr = mmap(NULL, 256, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if ((void *)-1 == memptr) {
		printf("mmap failed\n");
		close (fd);
		exit (-1);
	}

	map_array = (unsigned int *)memptr;
	msg = ((dest & 0xffff) << 16) + (cmd & 0xffff);

	for (i = 0; i < cnt; i++) {
		printf("ping #%d \n", i);
		map_array[Doorbell/sizeof(int)] = msg;
		sleep(1);
	}

	printf("Test end...\n");
	munmap(memptr, 256);
	close(fd);
}
