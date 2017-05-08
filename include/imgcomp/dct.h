/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef IMGCOMP_DCT_H
#define IMGCOMP_DCT_H

int image_fdct(struct image *original, struct imagef **processed);
int image_idct(struct imagef *original, struct image **processed);

#endif /* IMGCOMP_DCT_H */
