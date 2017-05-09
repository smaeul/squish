/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef IMGCOMP_ZIGZAG_H
#define IMGCOMP_ZIGZAG_H

int image_fzigzag(struct image *original, struct image **processed);
int image_izigzag(struct image *original, struct image **processed);

#endif /* IMGCOMP_ZIGZAG_H */
