/**
 * entropy.h - routines for calculating entropy
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef ENTROPY_H
#define ENTROPY_H

#ifdef __cplusplus
extern "C" {
#endif

#define ALPHABET_SIZE 256

/**
 *
 */
struct entctx {
	size_t counts[ALPHABET_SIZE];
	size_t inputsize;
};

/**
 * Allocate and initialize a structure for recording byte distributions.
 * @return A pointer to the structure, or NULL if allocation failed
 */
struct entctx *context_new();

/**
 * Free a structure previously allocated by context_new().
 * @param context A pointer to the structure
 * @return 0 if the operation was successful, or a negative value representing an error
 */
int context_free(struct entctx *context);

/**
 * Count the number of bytes with each value (0-255) in the buffer
 * @param context The context structure to update
 * @param buffer A pointer to a buffer to read data from
 * @param size The size of the buffer in bytes
 * @return 0 if the operation was successful, or a negative value representing an error
 */
int count_buffer(struct entctx *context, void *buffer, size_t size);

/**
 * Count the number of bytes with each value (0-255) to the end of the file
 * @param context The context structure to update
 * @param fd A seekable open file descriptor to read data from
 * @return 0 if the operation was successful, or a negative value representing an error
 */
int count_file(struct entctx *context, int fd);

#ifdef __cplusplus
}
#endif

#endif /* ENTROPY_H */
