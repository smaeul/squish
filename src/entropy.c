/**
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
	struct distribution *dist = 0;

	if (argc != 2 || !argv[1]) {
		fprintf(stderr, "entropy version %s\n%s\n\n"
		                "calculates entropy of a file\n"
		                "usage: %s <filename>\n",
		        VERSION, AUTHOR, argv[0]);
		return 1;
	}

	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		status = -errno;
		goto error;
	}
	if (!(dist = distribution_new())) {
		status = -errno;
		goto error;
	}
	if ((status = file_distribution(dist, fd)) < 0)
		goto error;
	if ((status = calculate_entropy(dist, &entropy)) < 0)
		goto error;
	printf("File '%s' has %.6f bits of entropy per character.\n", argv[1], entropy);

error:
	if (dist)
		distribution_free(dist);
	if (fd >= 0)
		close(fd);
	if (status < 0)
		fprintf(stderr, "Failed to calculate entropy: %s\n", strerror(-status));
	return -status;
}
