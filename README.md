# fast-memchr

This is a port of [rust-memchr]'s [fallback][original] and [SSE2]  `memchr()`
implementations to C.  It is distributed under the same licenses: MIT and
UNLICENSE.

In my limited tests, the fallback version ranges from about equal to 5.5x faster
than FreeBSD's stock `memchr()` (a simple bytewise loop) on amd64.

On i386 it's about 2.6x slower; 1.7x slower if `uintptr_t` is replaced with
`uint64_t`.

The SSE2 version is about 2.5-13x faster.


[rust-memchr]: https://github.com/BurntSushi/rust-memchr
[original]: https://github.com/BurntSushi/rust-memchr/blob/b0a29f267f4a7fad8ffcc8fe8377a06498202883/src/fallback.rs#L45-L81
[sse2]: https://github.com/BurntSushi/rust-memchr/blob/b0a29f267f4a7fad8ffcc8fe8377a06498202883/src/x86/sse2.rs#L18-L187
