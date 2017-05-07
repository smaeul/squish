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
	struct image *new;

	if (!img)
		return ERR_NULL;
	if (height >= SIZE_MAX / width || depth >= SIZE_MAX / (width * height))
		return ERR_INVAL;

	if (!(new = calloc(sizeof(struct image) + width * height * depth, 1)))
		return ERR_ALLOC;
	new->width = width;
	new->height = height;
	new->depth = depth;
	new->bytes = width *height *depth;
	*img = new;

	return 0;
}

void
image_free(struct image *img)
{
	if (!img)
		return;

	free(img);
}
