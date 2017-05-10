/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>

#include <imgcomp/imgcomp.h>

#define blockstart(image, row, col)                                                                \
	(((row) / BLOCKSIZE * ((image)->width / BLOCKSIZE) + (col) / BLOCKSIZE) *                      \
	 (BLOCKSIZE * BLOCKSIZE))

static int offsets[BLOCKSIZE][BLOCKSIZE] = {
	{ 0, 1, 5, 6, 14, 15, 27, 28 },     { 2, 4, 7, 13, 16, 26, 29, 42 },
	{ 3, 8, 12, 17, 25, 30, 41, 43 },   { 9, 11, 18, 24, 31, 40, 44, 53 },
	{ 10, 19, 23, 32, 39, 45, 52, 54 }, { 20, 22, 33, 38, 46, 51, 55, 60 },
	{ 21, 34, 37, 47, 50, 56, 59, 61 }, { 35, 36, 48, 49, 57, 58, 62, 63 }
};

int
image_fzigzag(struct image *original, struct image **processed)
{
	int err;
	int32_t dc;

	if (!original || !processed)
		return -EFAULT;
	if (original->width % BLOCKSIZE || original->height % BLOCKSIZE)
		return -ENOTSUP;

	if ((err = image_alloc(processed, original->width, original->height, original->depth)) < 0)
		goto out;
	/* Iterate over each block (rows/columns). */
	for (size_t a = 0; a < original->height; a += BLOCKSIZE) {
		for (size_t b = 0; b < original->width; b += BLOCKSIZE) {
			/* Get the starting position of this block in the pixelstream. */
			size_t start = blockstart(original, a, b);

			/* Iterate over each input pixel (rows/columns). */
			for (size_t i = 0; i < BLOCKSIZE; i += 1)
				for (size_t j = 0; j < BLOCKSIZE; j += 1)
					(*processed)->data[start + offsets[i][j]] = pixel(original, a + i, b + j);
			/* Perform DC prediction (difference from previous block's DC component). */
			if (a || b)
				(*processed)->data[start] -= dc;
			dc = pixel(original, a, b);
		}
	}
	err = 0;

out:
	return err;
}

int
image_izigzag(struct image *original, struct image **processed)
{
	int err;
	int32_t dc;

	if (!original || !processed)
		return -EFAULT;
	if (original->width % BLOCKSIZE || original->height % BLOCKSIZE)
		return -ENOTSUP;

	if ((err = image_alloc(processed, original->width, original->height, original->depth)) < 0)
		goto out;
	/* Iterate over each block (rows/columns). */
	for (size_t a = 0; a < original->height; a += BLOCKSIZE) {
		for (size_t b = 0; b < original->width; b += BLOCKSIZE) {
			/* Get the starting position of this block in the pixelstream. */
			size_t start = blockstart(original, a, b);

			/* Iterate over each input pixel (rows/columns). */
			for (size_t i = 0; i < BLOCKSIZE; i += 1)
				for (size_t j = 0; j < BLOCKSIZE; j += 1)
					pixel(*processed, a + i, b + j) = (original)->data[start + offsets[i][j]];
			/* Inverse DC prediction (add the previous block's DC component). */
			if (a || b)
				pixel(*processed, a, b) += dc;
			dc = pixel(*processed, a, b);
		}
	}
	err = 0;

out:
	return err;
}
