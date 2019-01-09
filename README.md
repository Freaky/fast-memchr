# fast-memchr

This is a port of [rust-memchr]'s [fallback `memchr()`][original] implementation
to C.  It is distributed under the same licenses: MIT and UNLICENSE.

In my limited tests, it's about 5x faster than FreeBSD's stock memchr() on amd64,
but about 2.6x slower on i386.  This is reduced to 1.7x if `uintptr_t` is
replaced with `uint64_t`.


[rust-memchr]: https://github.com/BurntSushi/rust-memchr
[original]: https://github.com/BurntSushi/rust-memchr/blob/b0a29f267f4a7fad8ffcc8fe8377a06498202883/src/fallback.rs#L45-L81
