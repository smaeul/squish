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

/**
 * Compress a buffer using a Huffman code. The output buffer will include the codebook.
 * @param buffer The data to compress
 * @param size The size of the uncompressed data in bytes
 * @param output A pointer wher the location of the output buffer will be stored on success.
 *				 This is a double pointer!
 * @param outsize An integer where the size of the output buffer will be stored on success.
 * @return 0 if the operation was successful, or a negative value representing an error
 */
long huffman_compress(void *buffer, size_t size, void *output, size_t *outsize);

/**
 * Compress a buffer using the given prefix code. The output buffer must already be allocated.
 * @param code The prefix code to use
 * @param buffer A buffer containing uncompressed data
 * @param size The size of the uncompressed data
 * @param output A buffer to fill with compressed data and the Huffman table
 * @param outsize The size of the output buffer
 * @return The number of bytes written to the output, or a negative value representing an error
 */
long prefix_encode(struct prefixcode *code, void *buffer, size_t size, void *output,
                   size_t outsize);

#ifdef __cplusplus
}
#endif

#endif /* SQUISH_PREFIXCODE_H */
