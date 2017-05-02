/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <imgcomp/imgcomp.h>

double
image_mse(struct image *original, struct image *processed)
{
	(void) original;
	(void) processed;

	return 0;
}

double
image_psnr(struct image *original, struct image *processed)
{
	(void) original;
	(void) processed;

	return 99;
}
