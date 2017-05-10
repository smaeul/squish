/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <imgcomp/imgcomp.h>

int
image_dump_block(struct image *img, size_t row, size_t col)
{
	if (!img)
		return -EFAULT;
	if (row >= img->height / BLOCKSIZE || col >= img->width / BLOCKSIZE)
		return -EINVAL;

	for (size_t i = 0; i < BLOCKSIZE; i += 1) {
		for (size_t j = 0; j < BLOCKSIZE; j += 1)
			printf("%10d ", pixel(img, BLOCKSIZE * row + i, BLOCKSIZE * col + j));
		putchar('\n');
	}

	return 0;
}

int
imagef_dump_block(struct imagef *img, size_t row, size_t col)
{
	if (!img)
		return -EFAULT;
	if (row >= img->height / BLOCKSIZE || col >= img->width / BLOCKSIZE)
		return -EINVAL;

	for (size_t i = 0; i < BLOCKSIZE; i += 1) {
		for (size_t j = 0; j < BLOCKSIZE; j += 1)
			printf("%10.0f ", pixel(img, BLOCKSIZE * row + i, BLOCKSIZE * col + j));
		putchar('\n');
	}

	return 0;
}
