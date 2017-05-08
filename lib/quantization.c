/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>

#include <imgcomp/imgcomp.h>

float image_quant_weights[BLOCKSIZE][BLOCKSIZE] = {
	{ 16, 11, 10, 16, 24, 40, 41, 61 },     { 12, 12, 14, 19, 26, 58, 60, 55 },
	{ 14, 13, 16, 24, 40, 57, 69, 56 },     { 14, 17, 22, 29, 51, 87, 80, 62 },
	{ 18, 22, 37, 56, 68, 109, 103, 77 },   { 24, 35, 55, 64, 81, 104, 113, 92 },
	{ 49, 64, 78, 87, 103, 121, 120, 101 }, { 72, 92, 95, 98, 112, 100, 103, 99 }
};

static void image_do_dequant(struct image *original, struct imagef *processed,
                             float steps[][BLOCKSIZE]);
static void image_do_quant(struct imagef *original, struct image *processed,
                           float steps[][BLOCKSIZE]);

int
image_dequant_unif(struct image *original, struct imagef **processed, float step)
{
	float steps[BLOCKSIZE][BLOCKSIZE];
	int err;

	if (!original || !processed)
		return ERR_NULL;
	if (!step)
		return ERR_INVAL;

	if ((err = image_allocf(processed, original->width, original->height, original->depth)) < 0)
		goto out;
	for (size_t i = 0; i < BLOCKSIZE; i += 1)
		for (size_t j = 0; j < BLOCKSIZE; j += 1)
			steps[i][j] = step;
	image_do_dequant(original, *processed, steps);
	err = 0;

out:
	return err;
}

int
image_dequant_weighted(struct image *original, struct imagef **processed, float step)
{
	float steps[BLOCKSIZE][BLOCKSIZE];
	int err;

	if (!original || !processed)
		return ERR_NULL;
	if (!step)
		return ERR_INVAL;

	if ((err = image_allocf(processed, original->width, original->height, original->depth)) < 0)
		goto out;
	for (size_t i = 0; i < BLOCKSIZE; i += 1)
		for (size_t j = 0; j < BLOCKSIZE; j += 1)
			steps[i][j] = image_quant_weights[i][j] * step;
	image_do_dequant(original, *processed, steps);
	err = 0;

out:
	return err;
}

static void
image_do_dequant(struct image *original, struct imagef *processed, float steps[][BLOCKSIZE])
{
	/* Iterate over each block (rows/columns). */
	for (size_t a = 0; a < original->height; a += BLOCKSIZE)
		for (size_t b = 0; b < original->width; b += BLOCKSIZE)
			/* Iterate over each pixel (rows/columns). */
			for (size_t i = 0; i < BLOCKSIZE; i += 1)
				for (size_t j = 0; j < BLOCKSIZE; j += 1)
					pixel(processed, a + i, b + j) = pixel(original, a + i, b + j) * steps[i][j];
}

static void
image_do_quant(struct imagef *original, struct image *processed, float steps[][BLOCKSIZE])
{
	/* Iterate over each block (rows/columns). */
	for (size_t a = 0; a < original->height; a += BLOCKSIZE)
		for (size_t b = 0; b < original->width; b += BLOCKSIZE)
			/* Iterate over each pixel (rows/columns). */
			for (size_t i = 0; i < BLOCKSIZE; i += 1)
				for (size_t j = 0; j < BLOCKSIZE; j += 1)
					pixel(processed, a + i, b + j) = pixel(original, a + i, b + j) / steps[i][j];
}

int
image_quant_unif(struct imagef *original, struct image **processed, float step)
{
	float steps[BLOCKSIZE][BLOCKSIZE];
	int err;

	if (!original || !processed)
		return ERR_NULL;
	if (!step)
		return ERR_INVAL;

	if ((err = image_alloc(processed, original->width, original->height, original->depth)) < 0)
		goto out;
	for (size_t i = 0; i < BLOCKSIZE; i += 1)
		for (size_t j = 0; j < BLOCKSIZE; j += 1)
			steps[i][j] = step;
	image_do_quant(original, *processed, steps);
	err = 0;

out:
	return err;
}

int
image_quant_weighted(struct imagef *original, struct image **processed, float step)
{
	float steps[BLOCKSIZE][BLOCKSIZE];
	int err;

	if (!original || !processed)
		return ERR_NULL;
	if (!step)
		return ERR_INVAL;

	if ((err = image_alloc(processed, original->width, original->height, original->depth)) < 0)
		goto out;
	for (size_t i = 0; i < BLOCKSIZE; i += 1)
		for (size_t j = 0; j < BLOCKSIZE; j += 1)
			steps[i][j] = image_quant_weights[i][j] * step;
	image_do_quant(original, *processed, steps);
	err = 0;

out:
	return err;
}
