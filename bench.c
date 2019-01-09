
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "memchr.h"

#define TEST_FILENAME "sherlock-holmes-huge.txt"
#define TEST_FILESIZE 594933
#define LOOPS 512

static size_t
fast_memchr_count(void *buf, int c, size_t len) {
	size_t count = 0;
	uint8_t *ptr = buf;
	uint8_t *end_ptr = ptr + len;

	while (ptr && ptr < end_ptr) {
		ptr = fast_memchr(ptr, c, len);
		if (ptr) {
			count++;
			ptr++;
		}
	}

	return count;
}

static size_t
libc_memchr_count(void *buf, int c, size_t len) {
	size_t count = 0;
	uint8_t *ptr = buf;
	uint8_t *end_ptr = ptr + len;

	while (ptr && ptr < end_ptr) {
		ptr = memchr(ptr, c, len);
		if (ptr) {
			count++;
			ptr++;
		}
	}

	return count;
}

static double
monotime_ms() {
	struct timespec now;
	if (clock_gettime(CLOCK_MONOTONIC, &now)) {
		exit(65);
	}
	return ((double)now.tv_sec * 1000) + ((double)now.tv_nsec / 1000000.0);
}

int main(void) {
	int fd = open(TEST_FILENAME, O_RDONLY);

	if (!fd) {
		return 1;
	}

	char *buf[TEST_FILESIZE];
	size_t len = read(fd, &buf, TEST_FILESIZE);
	(void)close(fd);

	if (len != TEST_FILESIZE) {
		return 64;
	}

	printf("char\tfast\tlibc\twinner\n");

	for (int i=0; i < 255; i++) {
		double start, end, fast_time, libc_time;
		int loop = LOOPS;
		int fast_count = 0;
		int libc_count = 0;

		printf("%d\t", i);

		start = monotime_ms();
		while (loop--) {
			fast_count += fast_memchr_count(&buf, i, len);
		}
		end = monotime_ms();
		fast_time = end - start;

		printf("%.3fms\t", fast_time);

		loop = LOOPS;
		start = monotime_ms();
		while (loop--) {
			libc_count += libc_memchr_count(&buf, i, len);
		}
		end = monotime_ms();
		libc_time = end - start;

		printf("%.3fms\t", libc_time);

		if (fast_time < libc_time) {
			printf("%.2fx faster", libc_time / fast_time);
		} else {
			printf("%.2fx slower", fast_time / libc_time);
		}

		printf("\n");

		if (fast_count != libc_count) {
			printf("counts differ: fast=%d != libc=%d\n", fast_count, libc_count);
			return 66;
		}
	}

	return 0;
}
