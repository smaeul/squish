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

#define C(x) ((x) ? 1.0 : 1.0 / sqrt(2))

/* Assumes square DCT */
#define DCT_SIZE 8

int
image_fdct(struct image *original, struct imagef **processed)
{
	int err;

	if (!original || !processed)
		return ERR_NULL;
	if (original->width % DCT_SIZE || original->height % DCT_SIZE)
		return ERR_NOTSUP;

	if ((err = image_allocf(processed, original->width, original->height)) < 0)
		goto out;
	/* Iterate over each block (rows/columns). */
	for (size_t a = 0; a < original->height; a += DCT_SIZE) {
		for (size_t b = 0; b < original->width; b += DCT_SIZE) {
			/* Iterate over each output pixel (rows/columns). */
			for (size_t k = 0; k < DCT_SIZE; k += 1) {
				for (size_t l = 0; l < DCT_SIZE; l += 1) {
					double sum = 0;

					/* Sum over cosines of the input pixels (rows/columns). */
					for (size_t m = 0; m < DCT_SIZE; m += 1) {
						for (size_t n = 0; n < DCT_SIZE; n += 1) {
							double cosx = cos((2 * m + 1) * k * M_PI / (2.0 * DCT_SIZE));
							double cosy = cos((2 * n + 1) * l * M_PI / (2.0 * DCT_SIZE));

							sum += cosx * cosy * pixel(original, a + m, b + n);
						}
					}
					sum *= (2.0 / DCT_SIZE) * C(k) * C(l);
					pixel(*processed, a + k, b + l) = sum;
				}
			}
		}
	}
	err = 0;

out:
	return err;
}

int
image_idct(struct imagef *original, struct image **processed)
{
	int err;

	if (!original || !processed)
		return ERR_NULL;
	if (original->width % DCT_SIZE || original->height % DCT_SIZE)
		return ERR_NOTSUP;

	if ((err = image_alloc(processed, original->width, original->height, 1)) < 0)
		goto out;
	/* Iterate over each block (rows/columns). */
	for (size_t a = 0; a < original->height; a += DCT_SIZE) {
		for (size_t b = 0; b < original->width; b += DCT_SIZE) {
			/* Iterate over each output pixel (rows/columns). */
			for (size_t m = 0; m < DCT_SIZE; m += 1) {
				for (size_t n = 0; n < DCT_SIZE; n += 1) {
					double sum = 0;

					/* Sum over cosines of the input pixels (rows/columns). */
					for (size_t k = 0; k < DCT_SIZE; k += 1) {
						for (size_t l = 0; l < DCT_SIZE; l += 1) {
							double cosx = cos((2 * m + 1) * k * M_PI / (2.0 * DCT_SIZE));
							double cosy = cos((2 * n + 1) * l * M_PI / (2.0 * DCT_SIZE));

							sum += cosx * cosy * pixel(original, a + k, b + l) * C(k) * C(l);
						}
					}
					sum *= 2.0 / DCT_SIZE;
					pixel(*processed, a + m, b + n) = sum;
				}
			}
		}
	}
	err = 0;

out:
	return err;
}
