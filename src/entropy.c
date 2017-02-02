/**
 * entropy.c - simple entropy calculator
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <squish/entropy.h>
#include <version.h>

int
main(int argc, char *argv[])
{
	double entropy;
	int fd, status = 0;
	struct entctx context = { 0 };

	if (argc != 2 || !argv[1]) {
		fprintf(stderr, "entropy version %s\n"
		                "calculates entropy of a file\n\n"
		                "usage: %s <filename>\n",
		        VERSION, argv[0]);
		return 1;
	}

	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		status = -errno;
		goto error;
	}
	if ((status = count_file(&context, fd)) < 0)
		goto error;
	printf("read %zu characters...\n", context.inputsize);
	if ((status = calc_entropy(&context, &entropy)) < 0)
		goto error;
	printf("File '%s' has %.6f bits of entropy per character.\n", argv[1], entropy);

error:
	close(fd);
	if (status)
		fprintf(stderr, "Failed to calculate entropy: %s\n", strerror(-status));
	return -status;
}
