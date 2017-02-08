/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef SQUISH_PREFIXCODE_H
#define SQUISH_PREFIXCODE_H

#include <stdint.h>

#include <squish/distribution.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A structure representing a codebook for a prefix code.
 */
struct prefixcode;

/**
 * Generate a Huffman code from the given distribution
 * @param dist The distribution of the data to generate a code for
 * @param code A pointer where the location of the codebook will be stored on success
 * @return 0 if the operation was successful, or a negative value representing an error
 */
long huffman_code(struct distribution *dist, struct prefixcode **code);

#ifdef __cplusplus
}
#endif

#endif /* SQUISH_PREFIXCODE_H */
