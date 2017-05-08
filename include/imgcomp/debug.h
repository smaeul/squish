/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef IMGCOMP_DEBUG_H
#define IMGCOMP_DEBUG_H

int image_dump_block(struct image *img, size_t row, size_t col);
int imagef_dump_block(struct imagef *img, size_t row, size_t col);

#endif /* IMGCOMP_DEBUG_H */
