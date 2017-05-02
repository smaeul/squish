/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef IMGCOMP_STATISTICS_H
#define IMGCOMP_STATISTICS_H

struct image_stats {
	double mse;  /**< The mean squared error of the processed image. */
	double psnr; /**< The signal-to-noise ratio of the processed image. */
};

double image_mse(struct image *original, struct image *processed);
double image_psnr(struct image *original, struct image *processed);

#endif /* IMGCOMP_STATISTICS_H */
