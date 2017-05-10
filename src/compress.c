/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <imgcomp/imgcomp.h>

int
main(int argc, char *argv[])
{
	char *infile, *outfile;
	int err = 111, infd, outfd;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <input> <output>\n", argv[0]);
		return 100;
	}

	infile = argv[1];
	outfile = argv[2];
	if ((infd = open(infile, O_RDONLY)) < 0) {
		perror("Cannot open input file");
		goto out;
	}
	if ((outfd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
		perror("Cannot open output file");
		goto out_close_infd;
	}

	printf("Compressing %s to %s\n", infile, outfile);
	err = -imagefile_compress(infd, outfd);

	close(outfd);
out_close_infd:
	close(infd);
out:
	return err;
}
