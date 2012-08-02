CFLAGS+=-pipe -O3 -Wall

all: chttp

chttp: chttp.o option.o

clean:
	-rm -f chttp *.o
