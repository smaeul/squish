/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef SQUISH_DISTRIBUTION_H
#define SQUISH_DISTRIBUTION_H

#include <limits.h>
#include <stddef.h>

#define ALPHABET_SIZE (1 << CHAR_BIT)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A structure representing a distribution of byte values.
 */
struct distribution {
	size_t counts[ALPHABET_SIZE]; /**< The number of bytes read with each value in the alphabet. */
	size_t total;                 /**< The total bytes of data read to create the distribution. */
};

/**
 * Count the number of bytes with each possible value in the buffer.
 * @param dist The distribution structure to update
 * @param buffer A pointer to a buffer to read data from
 * @param size The size of the buffer in bytes
 * @return 0 if the operation was successful, or a negative value representing an error
 */
long buffer_distribution(struct distribution *dist, void *buffer, size_t size);

/**
 * Adjust a distribution to ensure none of the symbols in the alphabet have a count of zero.
 * @param dist The distribution structure to update
 * @return 0 if the operation was successful, or a negative value representing an error
 */
long distribution_adjust(struct distribution *dist);

/**
 * Free a structure previously allocated by distribution_new().
 * @param dist A pointer to the structure
 * @return 0 if the operation was successful, or a negative value representing an error
 */
long distribution_free(struct distribution *dist);

/**
 * Allocate and initialize a structure for recording byte distributions.
 * @return A pointer to the structure, or NULL if allocation failed
 */
struct distribution *distribution_new();

/**
 * Count the number of bytes with each possible value to the end of the file.
 * @param dist The distribution structure to update
 * @param fd A seekable open file descriptor to read data from
 * @return 0 if the operation was successful, or a negative value representing an error
 */
long file_distribution(struct distribution *dist, int fd);

#ifdef __cplusplus
}
#endif

#endif /* SQUISH_DISTRIBUTION_H */
