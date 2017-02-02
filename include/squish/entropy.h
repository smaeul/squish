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

/**
 *
 */
struct entctx {
	size_t counts[256];
	size_t inputsize;
};

/**
 *
 */
struct entctx *context_new();

/**
 *
 */
int context_free(struct entctx *context);

/**
 *
 */
int count_buffer(struct entctx *context, void *buffer, size_t size);

/**
 *
 */
int count_file(struct entctx *context, int fd);

#ifdef __cplusplus
}
#endif

#endif /* ENTROPY_H */
