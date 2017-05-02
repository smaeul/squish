/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef IMGCOMP_IMGCOMP_H
#define IMGCOMP_IMGCOMP_H

#include <imgcomp/image.h>
#include <imgcomp/statistics.h>

int imagefile_compare(int origfd, int procfd, struct image_stats *stats);
int imagefile_compress(int infd, int outfd);
int imagefile_decompress(int infd, int outfd);

#endif /* IMGCOMP_IMGCOMP_H */
