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
 * Convert a 64-bit integer from host to network byte order.
 * @param host An integer in host byte order
 * @return The integer in network byte order
 */
uint64_t htonll(uint64_t host);

/**
 * Calculate the logarithm base 2 of the argument using integer math. The result is only dependent
 * on the position of the first set bit, making this calculation the equivalent of floor(log2(n)).
 * @param n The number to take the logarithm of
 * @return The integer floor of the logarithm base 2 of the number, or 0 if the number is 0.
 */
uint32_t log2u32(uint32_t n);

/**
 * Swap n bytes of data between two blocks of memory.
 * @param a The first block of memory
 * @param b The second block of memory
 * @param n The number of bytes to swap
 */
void memswap(void *a, void *b, size_t n);

/**
 * Convert a 64-bit integer from network to host byte order.
 * @param net An integer in network byte order
 * @return The integer in host byte order
 */
uint64_t ntohll(uint64_t net);

#ifdef __cplusplus
}
#endif

#endif /* INTERNAL_UTILS_H */
