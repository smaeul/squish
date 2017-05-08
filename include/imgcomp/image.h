/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef IMGCOMP_IMAGE_H
#define IMGCOMP_IMAGE_H

#define IMAGE_MAXDEPTH sizeof(_dummy_image.data[0])

#define pixel(image, x, y) (image)->data[(x) * (image)->width + (y)]

struct image {
	size_t width;    /**< The width of the image in pixels. */
	size_t height;   /**< The height of the image in pixels. */
	size_t depth;    /**< The number of bytes used per pixel in the file. */
	size_t bytes;    /**< The length of the image file in bytes. */
	uint32_t data[]; /**< The stream of bytes representing the image. */
};

struct imagef {
	size_t width;  /**< The width of the image in pixels. */
	size_t height; /**< The height of the image in pixels. */
	size_t depth;  /**< The number of bytes used per pixel in the file. */
	size_t bytes;  /**< The length of the image file in bytes. */
	float data[];  /**< Floating-point values representing the image pixels. */
};

extern struct image _dummy_image;

int image_alloc(struct image **img, size_t width, size_t height, size_t depth);
void image_free(struct image *img);

static inline int
image_allocf(struct imagef **img, size_t width, size_t height)
{
	return image_alloc((struct image **) img, width, height, IMAGE_MAXDEPTH);
}

static inline void
image_freef(struct imagef *img)
{
	image_free((struct image *) img);
}

#endif /* IMGCOMP_IMAGE_H */
