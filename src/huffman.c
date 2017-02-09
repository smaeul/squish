/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <internal/io.h>
#include <internal/utils.h>
#include <squish/prefixcode.h>
#include <version.h>

int
main(int argc, char *argv[])
{
	char outfile[PATH_MAX];
	int fd, status = 0;
	off_t size;
	size_t compsize;
	uint64_t headersize;
	void *buffer, *compressed = 0;

	if (argc != 2 || !argv[1]) {
		fprintf(stderr, "huffman version %s\n%s\n\n"
		                "generates huffman code for a file and compresses it to <filename>.sq\n"
		                "usage: %s <filename>\n",
		        VERSION, AUTHOR, argv[0]);
		return 1;
	}
	if (strlen(argv[1]) >= PATH_MAX - 3) {
		fprintf(stderr, "file name '%s' too long\n", argv[1]);
		return 1;
	}
	strcpy(outfile, argv[1]);
	strcat(outfile, ".sq");

	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		status = -errno;
		goto error;
	}
	/* Find the size of the file. */
	if ((size = lseek(fd, 0, SEEK_END)) == -1) {
		status = -errno;
		goto error;
	}
	if ((buffer = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
		status = -errno;
		goto error;
	}
	/* The file can be closed as soon as the memory mapping is made. fd will be reused. */
	close(fd);
	printf("Calculating symbol distribution and generating Huffman code...\n");
	if ((status = huffman_compress(buffer, size, &compressed, &compsize)) < 0)
		goto error;
	printf("Huffman code is as follows:\n");
	fflush(stdout);
	/* XXX: This is a hack and is not safe, but it works well enough for now. */
	if ((status = prefix_printcode((struct prefixcode *) compressed, STDOUT_FILENO)) < 0)
		goto error;
	/* Overhead is the file header (24 bytes, not in compsize) plus the codebook (1024 bytes,
	 * included in compsize). */
	printf("Compression ratio (excluding overhead) is %.4fx.\n",
	       (double) size / ((double) compsize - 1024));
	printf("Compression ratio (including overhead) is %.4fx.\n",
	       (double) size / ((double) compsize + 24));
	if ((munmap(buffer, size)) == -1) {
		status = -errno;
		goto error;
	}

	printf("Writing output file %s...\n", outfile);
	if ((fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
		status = -errno;
		goto error;
	}
	/* Write the header. */
	if ((status = write_retry(fd, "squish\0\0", 8, 1)) < 8) {
		if (status >= 0)
			status = -errno;
		goto error;
	}
	/* Convert the size to network byte order and write it. */
	headersize = htonll(compsize);
	if ((status = write_retry(fd, &headersize, 8, 1)) < 8) {
		if (status >= 0)
			status = -errno;
		goto error;
	}
	/* Skip over the reserved section. */
	if (lseek(fd, 8, SEEK_CUR) == -1) {
		status = -errno;
		goto error;
	}
	/* Write the data. */
	if ((status = write_retry(fd, compressed, compsize, 1)) < (ssize_t) compsize) {
		if (status >= 0)
			status = -errno;
		goto error;
	}
	status = 0;
	printf("Compression was successful.\n");

error:
	if (compressed)
		free(compressed);
	if (fd >= 0)
		close(fd);
	if (status < 0)
		fprintf(stderr, "Failed to compress file: %s\n", strerror(-status));
	return -status;
}
