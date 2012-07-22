CC		= gcc
CFLAGS	= -Iinclude -O2 -DNDEBUG -std=c99

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

#
# Default target

OBJ		=	src/config.o \
			src/util.o

obj: $(OBJS)

clean:
	find . -name '*.o' -exec rm {} \;
	rm -rf build

#
# Build

# build/test: obj
# 	if [ ! -d build ]; then mkdir build; fi
# 	$(CC) $(CFLAGS) -o build/test src/test.c $(OBJS) $(LDFLAGS)
