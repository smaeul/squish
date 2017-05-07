/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef IMGCOMP_IMAGE_H
#define IMGCOMP_IMAGE_H

struct image {
	size_t width;   /**< The width of the image in pixels. */
	size_t height;  /**< The height of the image in pixels. */
	size_t depth;   /**< The number of bytes used per pixel. */
	size_t bytes;   /**< The length of the image data in bytes. */
	uint8_t data[]; /**< The stream of bytes representing the image. */
};

int image_alloc(struct image **img, size_t width, size_t height, size_t depth);
void image_free(struct image *img);

#endif /* IMGCOMP_IMAGE_H */
