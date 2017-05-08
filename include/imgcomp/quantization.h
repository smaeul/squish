/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef IMGCOMP_QUANTIZATION_H
#define IMGCOMP_QUANTIZATION_H

int image_dequant_unif(struct image *original, struct imagef **processed, float step);
int image_dequant_weighted(struct image *original, struct imagef **processed, float step);
int image_quant_unif(struct imagef *original, struct image **processed, float step);
int image_quant_weighted(struct imagef *original, struct image **processed, float step);

#endif /* IMGCOMP_QUANTIZATION_H */
