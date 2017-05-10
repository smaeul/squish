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

#include <imgcomp/imgcomp.h>

double
image_mse(struct image *original, struct image *processed)
{
	double mse;

	if (!original || !processed)
		return -EFAULT;
	if (original->height != processed->height || original->width != processed->width)
		return -EINVAL;

	mse = 0;
	for (size_t i = 0; i < original->height; i += 1) {
		for (size_t j = 0; j < original->width; j += 1) {
			/* Only compare the appropriate number of bytes (not interpolated extension). */
			int32_t opx = pixel(original, i, j) >> (IMAGE_MAXDEPTH - original->depth) * CHAR_BIT;
			int32_t ppx = pixel(processed, i, j) >> (IMAGE_MAXDEPTH - original->depth) * CHAR_BIT;
			mse += pow(opx - ppx, 2);
		}
	}
	mse /= original->height * original->width;

	return mse;
}

double
image_psnr(struct image *original, struct image *processed)
{
	double maxval, mse, psnr;

	if (!original || !processed)
		return -EFAULT;
	if (original->height != processed->height || original->width != processed->width)
		return -EINVAL;

	if ((mse = image_mse(original, processed)) < 0)
		return mse;
	if (mse > 0) {
		maxval = (1UL << (original->depth * CHAR_BIT)) - 1;
		psnr = 10 * log10(pow(maxval, 2) / mse);
	} else {
		psnr = 99;
	}

	return psnr;
}
