/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef IMGCOMP_UTILS_H
#define IMGCOMP_UTILS_H

uint8_t log2i32(int32_t n);
uint8_t log2u32(uint32_t n);
int readall(int fd, void *buffer, size_t size);
int writeall(int fd, void *buffer, size_t size);

#endif /* IMGCOMP_UTILS_H */
