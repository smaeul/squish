/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef IMGCOMP_UTILS_H
#define IMGCOMP_UTILS_H

int readall(int fd, uint8_t *buffer, size_t size);
int writeall(int fd, uint8_t *buffer, size_t size);

#endif /* IMGCOMP_UTILS_H */
