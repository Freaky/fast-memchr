
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "memchr.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define LOOP_SIZE (2 * sizeof(uintptr_t))

// Return `true` if `x` contains any zero byte.
//
// From *Matters Computational*, J. Arndt
//
// "The idea is to subtract one from each of the bytes and then look for
// bytes where the borrow propagated all the way to the most significant
// bit."
static bool
contains_zero_byte(uintptr_t x) {
	return(((x - ((uintptr_t)0x0101010101010101L)) & ~x &
	             ((uintptr_t)0x8080808080808080L)) != 0);
}

// Repeat the given byte into a word size number. That is, every 8 bits
// is equivalent to the given byte. For example, if `b` is `\x4E` or
// `01001110` in binary, then the returned value on a 32-bit system would be:
// `01001110_01001110_01001110_01001110`.
static uintptr_t
repeat_byte(uint8_t b) {
	return(((uintptr_t) b) * (UINTPTR_MAX / 255));
}


static uintptr_t
read_unaligned_word(uint8_t *ptr) {
	uintptr_t ret;
	memcpy(&ret, ptr, sizeof(uintptr_t));
	return ret;
}

static void *
forward_search(uint8_t *ptr, uint8_t *end_ptr, uint8_t n) {
	while (ptr <= end_ptr) {
		if (*ptr == n) {
			return (void *)ptr;
		}
		ptr++;
	}

	return NULL;
}

void *
fast_memchr(void *haystack, int n, size_t len) {
	uint8_t n1 = (uint8_t) n;
	uintptr_t vn1 = repeat_byte(n1);
	uint32_t loop_size = MIN(LOOP_SIZE, len);
	uintptr_t align = sizeof(uintptr_t) - 1;

	uint8_t *start_ptr = haystack;
	uint8_t *end_ptr = start_ptr + (len - 1);
	uint8_t *ptr = start_ptr;

	if (len < sizeof(uintptr_t)) {
		return forward_search(ptr, end_ptr, n1);
	}

	uintptr_t chunk = read_unaligned_word(ptr);
	if (contains_zero_byte(chunk ^ vn1)) {
		return forward_search(ptr, end_ptr, n1);
	}

	ptr += sizeof(uintptr_t) - (((uintptr_t)start_ptr) & align);

	while (loop_size == LOOP_SIZE && ptr <= (end_ptr - loop_size)) {
		uintptr_t a, b;
		memcpy(&a, ptr, sizeof(uintptr_t));
		memcpy(&b, ptr + sizeof(uintptr_t), sizeof(uintptr_t));
		bool eqa = contains_zero_byte(a ^ vn1);
		bool eqb = contains_zero_byte(b ^ vn1);

		if (eqa || eqb) {
			break;
		}
		ptr += LOOP_SIZE;
	}

	return forward_search(ptr, end_ptr, n1);
}
