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

	if ((err = imagefile_read(origfd, &original)) < 0)
		goto out;
	if ((err = imagefile_read(procfd, &processed)) < 0)
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
	struct image *img;

	if ((err = imagefile_read(infd, &img)) < 0)
		goto out;
	if ((err = imagefile_write(outfd, img)) < 0)
		goto out_free_img;
	err = 0;

out_free_img:
	image_free(img);
out:
	return err;
}

int
imagefile_decompress(int infd, int outfd)
{
	int err;
	struct image *img;

	if ((err = imagefile_read(infd, &img)) < 0)
		goto out;
	if ((err = imagefile_write(outfd, img)) < 0)
		goto out_free_img;
	err = 0;

out_free_img:
	image_free(img);
out:
	return err;
}

int
imagefile_read(int fd, struct image **img)
{
	int err;

	if (!img)
		return ERR_NULL;
	if ((err = image_alloc(img, IMGSIZE, IMGSIZE, IMGDEPTH)) < 0)
		goto out;
	if ((err = readall(fd, (*img)->data, (*img)->bytes)) < 0) {
		image_free(*img);
		goto out;
	}
	err = 0;

out:
	return err;
}

int
imagefile_write(int fd, struct image *img)
{
	int err;

	if (!img)
		return ERR_NULL;
	if ((err = writeall(fd, img->data, img->bytes)) < 0)
		goto out;
	err = 0;

out:
	return err;
}
