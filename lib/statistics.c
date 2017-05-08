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

#include <imgcomp/imgcomp.h>

double
image_mse(struct image *original, struct image *processed)
{
	double mse;

	if (!original || !processed)
		return ERR_NULL;
	if (original->width != processed->width || original->height != processed->height)
		return ERR_INVAL;
	if (original->depth != processed->depth || original->bytes != processed->bytes)
		return ERR_NOTSUP;
	/* For right now, all relevant images use one byte per pixel. */
	if (original->depth != 1 || processed->depth != 1)
		return ERR_NOTSUP;

	mse = 0;
	for (size_t i = 0; i < original->bytes; i += 1)
		mse += pow((double) original->data[i] - processed->data[i], 2);
	mse /= original->bytes;

	return mse;
}

double
image_psnr(struct image *original, struct image *processed)
{
	double maxval, mse, psnr;

	if (!original || !processed)
		return ERR_NULL;
	if (original->depth != processed->depth)
		return ERR_NOTSUP;

	if ((mse = image_mse(original, processed)) < 0)
		return mse;
	if (mse > 0) {
		maxval = 1UL << (original->depth * CHAR_BIT);
		psnr = 10 * log10(maxval * maxval / mse);
	} else {
		psnr = 99;
	}

	return psnr;
}
