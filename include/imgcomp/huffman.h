/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef IMGCOMP_HUFFMAN_H
#define IMGCOMP_HUFFMAN_H

extern size_t totalread;
extern size_t totalwritten;

int imagefile_read_huffman(int fd, size_t depth, struct image **img);
int imagefile_write_huffman(int fd, struct image *img);

#endif /* IMGCOMP_HUFFMAN_H */
