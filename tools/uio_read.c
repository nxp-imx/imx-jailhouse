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

int main(int argc, char ** argv)
{
	int cnt, rv, fd, buf;

	if (argc != 3) {
		printf("USAGE: %s <filename> <count>\n", argv[0]);
		exit(-1);
	}

	cnt = atoi(argv[2]);
	fd = open(argv[1], O_RDWR);
	if(fd < 0){
		printf("error on open %s\n", argv[1]);
		exit(-1);
	}

	while (cnt--) {
		rv = read(fd, &buf, sizeof(buf));
		printf("buf is %d, len %d\n", buf, rv);
    }

	printf("Test end...\n");
	close(fd);
}
