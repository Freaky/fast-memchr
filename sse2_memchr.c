#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <emmintrin.h>

#include "memchr.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

#define VECTOR_SIZE (sizeof(__m128i))
#define VECTOR_ALIGN (VECTOR_SIZE - 1)
#define LOOP_SIZE (4 * VECTOR_SIZE)

static uintptr_t
forward_pos(uint32_t mask) {
	return __builtin_ctz(mask);
}

static void *
forward_search1(uint8_t *ptr, __m128i vn1) {
	__m128i chunk = _mm_loadu_si128((__m128i *)ptr);
	uint32_t mask = _mm_movemask_epi8(_mm_cmpeq_epi8(chunk, vn1));

	if (mask != 0) {
		return (void *)(ptr + forward_pos(mask));
	} else {
		return NULL;
	}
}

void *
fast_memchr(void *haystack, int n, size_t len) {
	__m128i vn1 = _mm_set1_epi8((uint8_t) n);
	uint32_t loop_size = MIN(LOOP_SIZE, len);

	uint8_t *start_ptr = haystack;
	uint8_t *end_ptr = start_ptr + len;
	uint8_t *ptr = start_ptr;

	if (len < VECTOR_SIZE) {
		while (ptr < end_ptr) {
			if (*ptr == n) {
				return (void *)ptr;
			}

			ptr++;
		}
		return NULL;
	}

	void *pos;
	if ((pos = forward_search1(ptr, vn1))) {
		return pos;
	}

	ptr += VECTOR_SIZE - (((uintptr_t)start_ptr) & VECTOR_ALIGN);

	assert(ptr > start_ptr && (end_ptr - VECTOR_SIZE) >= start_ptr);
	while (loop_size == LOOP_SIZE && ptr <= (end_ptr - loop_size)) {
		assert((uintptr_t)ptr % VECTOR_SIZE == 0);
		__m128i a = _mm_load_si128((__m128i *)ptr);
		__m128i b = _mm_load_si128((__m128i *)(ptr + VECTOR_SIZE));
		__m128i c = _mm_load_si128((__m128i *)(ptr + (2 * VECTOR_SIZE)));
		__m128i d = _mm_load_si128((__m128i *)(ptr + (3 * VECTOR_SIZE)));

		__m128i eqa = _mm_cmpeq_epi8(vn1, a);
		__m128i eqb = _mm_cmpeq_epi8(vn1, b);
		__m128i eqc = _mm_cmpeq_epi8(vn1, c);
		__m128i eqd = _mm_cmpeq_epi8(vn1, d);

		__m128i or1 = _mm_or_si128(eqa, eqb);
		__m128i or2 = _mm_or_si128(eqc, eqd);
		__m128i or3 = _mm_or_si128(or1, or2);

		if (_mm_movemask_epi8(or3) != 0) {
			uint8_t *at = ptr;
			uint32_t mask = _mm_movemask_epi8(eqa);

			if (mask != 0) {
				return (void *)(at + forward_pos(mask));
			}

			at += VECTOR_SIZE;
			mask = _mm_movemask_epi8(eqb);

			if (mask != 0) {
				return (void *)(at + forward_pos(mask));
			}

			at += VECTOR_SIZE;
			mask = _mm_movemask_epi8(eqc);

			if (mask != 0) {
				return (void *)(at + forward_pos(mask));
			}

			at += VECTOR_SIZE;
			mask = _mm_movemask_epi8(eqd);

			assert(mask != 0);

			return (void *)(at + forward_pos(mask));
		}

		ptr += loop_size;
	}

	while (ptr <= (end_ptr - VECTOR_SIZE)) {
		assert(end_ptr - ptr >= VECTOR_SIZE);
		if ((pos = forward_search1(ptr, vn1))) {
			return pos;
		}

		ptr += VECTOR_SIZE;
	}

	if (ptr < end_ptr) {
		assert(end_ptr - ptr < VECTOR_SIZE);
		ptr -= VECTOR_SIZE - (end_ptr - ptr);
		assert(end_ptr - ptr == VECTOR_SIZE);

		return forward_search1(ptr, vn1);
	}

	return NULL;
}
