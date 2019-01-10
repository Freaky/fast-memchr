PROG=bench
SRCS=bench.c memchr.c
SSE2_SRCS=bench.c sse2_memchr.c

CFLAGS=-O2 -std=gnu99 -msse2 -Wall

OBJ=${SRCS:.c=.o}
SSE2OBJ=${SSE2_SRCS:.c=.o}

all: ${PROG} sse2

sse2: ${PROG}.sse2

${PROG}.sse2: ${SSE2OBJ}
	${CC} ${CFLAGS} -o $@ ${SSE2OBJ} ${LDFLAGS}

${PROG}: ${OBJ}
	${CC} ${CFLAGS} -o $@ ${OBJ} ${LDFLAGS}

%.o: %.c
	${CC} ${CFLAGS} -o $@ -c $<

clean:
	rm -f ${PROG} ${PROG}.sse2 ${OBJ} ${SSE2OBJ}

