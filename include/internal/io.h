/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef INTERNAL_IO_H
#define INTERNAL_IO_H

#include <stddef.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @param minimum Only retry if at least this many bytes are read with each call to read(3)
 *
 */
ssize_t read_retry(int fd, void *buffer, size_t size, size_t minimum);

/**
 * Write a buffer to a file, retrying until the entire buffer is written.
 * @param fd The file to write to
 * @param buffer The data to write to the file
 * @param size The size of the data to write
 * @param minimum Only retry if at least this many bytes are written with each call to write(3)
 * @return The number of bytes written to the file, or a negative value representing an error if no
 *         bytes were able to be written. Note that the whole purpose of this function is to handle
 *         short writes, so a return value not equal to 'size' should be considered an error.
 */
ssize_t write_retry(int fd, void *buffer, size_t size, ssize_t minimum);

#ifdef __cplusplus
}
#endif

#endif /* INTERNAL_IO_H */
