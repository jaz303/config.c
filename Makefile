CC			= gcc
CFLAGS		= -Isrc -Iinclude -O2 -DNDEBUG -std=c99

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

#
# Default target

default: build/config

APPLICATION_OBJS = 	src/object.o \
					src/parser.o \
					src/pool.o \
					src/util.o

DEBUG_OBJS = src/debug.o

OBJS = $(APPLICATION_OBJS) $(DEBUG_OBJS)

obj: $(OBJS)

clean:
	find . -name '*.o' -exec rm {} \;
	rm -rf build

#
# Build

build/test: obj
	if [ ! -d build ]; then mkdir build; fi
	$(CC) $(CFLAGS) -o build/test src/test.c $(OBJS) $(LDFLAGS)
