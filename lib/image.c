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

int
image_alloc(struct image **img, size_t width, size_t height, size_t depth)
{
	struct image *newimg;

	if (!img)
		return ERR_NULL;
	if (height >= SIZE_MAX / width || depth >= SIZE_MAX / (width * height))
		return ERR_INVAL;
	if (depth > IMAGE_MAXDEPTH)
		return ERR_INVAL;

	if (!(newimg = calloc(sizeof(struct image) + width * height * IMAGE_MAXDEPTH, 1)))
		return ERR_ALLOC;
	newimg->width = width;
	newimg->height = height;
	newimg->depth = depth;
	newimg->bytes = width * height * depth;
	*img = newimg;

	return 0;
}

void
image_free(struct image *img)
{
	if (!img)
		return;

	free(img);
}
