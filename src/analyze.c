/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <imgcomp/imgcomp.h>

int
main(int argc, char *argv[])
{
	char *infile, *tmpfile = strdup("/tmp/image.XXXXXX");
	int childerr = 0, err = 111, infd, pipefd[2], tmpfd, wstat = -1;
	pid_t compressor, decompressor;
	struct image_stats stats;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <image>\n", argv[0]);
		return 100;
	}

	infile = argv[1];
	if ((infd = open(infile, O_RDONLY)) < 0) {
		perror("Cannot open input file");
		goto out;
	}
	if (pipe(pipefd) < 0) {
		perror("Cannot open pipe");
		goto out_close_infd;
	}
	if ((tmpfd = mkstemp(tmpfile)) < 0) {
		perror("Cannot open temporary file");
		goto out_close_pipe;
	}

	printf("Analyzing compression performance using %s\n", infile);
	if ((compressor = fork()) < 0) {
		perror("Cannot fork compressor");
		goto out_close_tmpfd;
	} else if (compressor == 0) {
		/* Compressor child */
		err = -imagefile_compress(infd, pipefd[1]);
		/* TODO: time compression. */
		goto out_close_tmpfd;
	}
	if ((decompressor = fork()) < 0) {
		perror("Cannot fork decompressor");
		goto out_close_tmpfd;
	} else if (decompressor == 0) {
		/* Decompressor child */
		err = -imagefile_decompress(pipefd[0], tmpfd);
		/* TODO: time decompression. */
		goto out_close_tmpfd;
	}

	while (waitpid(compressor, &wstat, 0) != compressor)
		if (errno != EINTR)
			break;
	if (!(WIFEXITED(wstat) && WEXITSTATUS(wstat) == 0)) {
		if (WIFEXITED(wstat))
			fprintf(stderr, "Compression failed: %s\n", strerror(WEXITSTATUS(wstat)));
		else
			fprintf(stderr, "Compression failed: child exited abnormally\n");
		childerr = 1;
	}
	while (waitpid(decompressor, &wstat, 0) != decompressor)
		if (errno != EINTR)
			break;
	if (!(WIFEXITED(wstat) && WEXITSTATUS(wstat) == 0)) {
		if (WIFEXITED(wstat))
			fprintf(stderr, "Decompression failed: %s\n", strerror(WEXITSTATUS(wstat)));
		else
			fprintf(stderr, "Decompression failed: child exited abnormally\n");
		childerr = 1;
	}
	if (childerr)
		goto out_close_tmpfd;

	printf("Comparing processed image to the original\n");
	if (lseek(infd, 0, SEEK_SET) < 0) {
		perror("Cannot return to start of input file");
		goto out_close_tmpfd;
	}
	if (lseek(tmpfd, 0, SEEK_SET) < 0) {
		perror("Cannot return to start of temporary file");
		goto out_close_tmpfd;
	}
	err = -imagefile_compare(infd, tmpfd, &stats);
	printf("Mean squared error of processed image: %0.2f levels²/pixel²\n", stats.mse);
	printf("pSNR of processed image: %0.2f dB\n", stats.psnr);

out_close_tmpfd:
	close(tmpfd);
out_close_pipe:
	close(pipefd[0]);
	close(pipefd[1]);
out_close_infd:
	close(infd);
out:
	return err;
}
