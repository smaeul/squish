/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef IMGCOMP_IMGCOMP_H
#define IMGCOMP_IMGCOMP_H

#include <imgcomp/error.h>
#include <imgcomp/image.h>

#include <imgcomp/dct.h>
#include <imgcomp/debug.h>
#include <imgcomp/quantization.h>
#include <imgcomp/statistics.h>
#include <imgcomp/utils.h>

/* Requires square blocks. */
#define BLOCKSIZE 8

/* Properties of the images used for testing (since they have no header). */
#define TEST_DEPTH 1
#define TEST_SIZE 512

int imagefile_compare(int origfd, int procfd, struct image_stats *stats);
int imagefile_compress(int infd, int outfd);
int imagefile_decompress(int infd, int outfd);
int imagefile_read(int fd, size_t depth, struct image **img);
int imagefile_read_raw(int fd, size_t depth, struct image **img);
int imagefile_write(int fd, struct image *img);
int imagefile_write_raw(int fd, struct image *img);

static inline int
imagefile_readf(int fd, struct imagef **img)
{
	return imagefile_read_raw(fd, IMAGE_MAXDEPTH, (struct image **) img);
}

static inline int
imagefile_writef(int fd, struct imagef *img)
{
	return imagefile_write_raw(fd, (struct image *) img);
}

#endif /* IMGCOMP_IMGCOMP_H */
