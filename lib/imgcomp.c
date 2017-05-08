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
#include <string.h>

#include <imgcomp/imgcomp.h>

#define IMGDEPTH 1
#define IMGSIZE 512

int
imagefile_compare(int origfd, int procfd, struct image_stats *stats)
{
	double mse, psnr;
	int err;
	struct image *original, *processed;

	if (!stats)
		return ERR_NULL;

	if ((err = imagefile_read(origfd, IMGDEPTH, &original)) < 0)
		goto out;
	if ((err = imagefile_read(procfd, IMGDEPTH, &processed)) < 0)
		goto out_free_original;
	if ((mse = image_mse(original, processed)) < 0) {
		err = mse;
		goto out_free_processed;
	}
	if ((psnr = image_psnr(original, processed)) < 0) {
		err = psnr;
		goto out_free_processed;
	}
	stats->mse = mse;
	stats->psnr = psnr;
	err = 0;

out_free_processed:
	image_free(processed);
out_free_original:
	image_free(original);
out:
	return err;
}

int
imagefile_compress(int infd, int outfd)
{
	int err;
	struct image *img1, *img2;

	if ((err = imagefile_read(infd, IMGDEPTH, &img1)) < 0)
		goto out;
	if ((err = image_fdct(img1, &img2)) < 0)
		goto out_free_img1;
	if ((err = imagefile_write(outfd, img2)) < 0)
		goto out_free_img2;
	err = 0;

out_free_img2:
	image_free(img2);
out_free_img1:
	image_free(img1);
out:
	return err;
}

int
imagefile_decompress(int infd, int outfd)
{
	int err;
	struct image *img1, *img2;

	/* Make sure no processing is done to the bitstream when reading the file. */
	if ((err = imagefile_read(infd, IMAGE_MAXDEPTH, &img1)) < 0)
		goto out;
	if ((err = image_idct(img1, &img2)) < 0)
		goto out_free_img1;
	/* Use the expected bit depth when writing out the image. */
	img2->depth = IMGDEPTH;
	if ((err = imagefile_write(outfd, img2)) < 0)
		goto out_free_img2;
	err = 0;

out_free_img2:
	image_free(img2);
out_free_img1:
	image_free(img1);
out:
	return err;
}

int
imagefile_read(int fd, size_t depth, struct image **img)
{
	int err;
	struct image *newimg;

	if (!img)
		return ERR_NULL;
	if (!depth || depth > IMAGE_MAXDEPTH)
		return ERR_INVAL;

	if ((err = image_alloc(&newimg, IMGSIZE, IMGSIZE, depth)) < 0)
		goto out;
	if ((err = readall(fd, newimg->data, newimg->bytes)) < 0) {
		image_free(newimg);
		goto out;
	}
	/* Expand data to fill the buffer. Assumes little-endian. */
	if (depth < IMAGE_MAXDEPTH) {
		uint8_t *buffer = (uint8_t *) newimg->data;

		/* Treat the first pixel separately, since memcpy cannot overlap. */
		for (size_t i = newimg->bytes - 1; i > 0; i -= 1) {
			/* We must clear the padding bytes anyway, so copy all four bytes first. */
			memcpy(buffer + IMAGE_MAXDEPTH * (i + 1) - depth, buffer + depth * i, depth);
			memset(buffer + IMAGE_MAXDEPTH * i, 0, IMAGE_MAXDEPTH - depth);
		}
		newimg->data[0] <<= (IMAGE_MAXDEPTH - depth) * CHAR_BIT;
	}
	*img = newimg;
	err = 0;

out:
	return err;
}

int
imagefile_write(int fd, struct image *img)
{
	int err;
	uint8_t *buffer;

	if (!img)
		return ERR_NULL;

	/* Write out the significant bytes for each pixel. Assumes little-endian. */
	buffer = (uint8_t *) img->data;
	for (size_t i = 0; i < img->height * img->width; i += 1)
		if ((err = writeall(fd, buffer + IMAGE_MAXDEPTH * (i + 1) - img->depth, img->depth)) < 0)
			goto out;
	err = 0;

out:
	return err;
}
