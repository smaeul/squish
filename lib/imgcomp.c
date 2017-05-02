/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <imgcomp/imgcomp.h>

int
imagefile_compare(int origfd, int procfd, struct image_stats *stats)
{
	(void) origfd;
	(void) procfd;

	stats->mse = 0;
	stats->psnr = 99;

	return 0;
}

int
imagefile_compress(int infd, int outfd)
{
	(void) infd;
	(void) outfd;

	return 0;
}

int
imagefile_decompress(int infd, int outfd)
{
	(void) infd;
	(void) outfd;

	return 0;
}
