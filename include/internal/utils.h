/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef INTERNAL_UTILS_H
#define INTERNAL_UTILS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Swap n bytes of data between two blocks of memory.
 * @param a The first block of memory
 * @param b The second block of memory
 * @param n The number of bytes to swap
 */
void memswap(void *a, void *b, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* INTERNAL_UTILS_H */
