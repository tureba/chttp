CFLAGS += -pipe -O3 -Wall
LDFLAGS += -lmagic

all: chttp

chttp: chttp.o option.o magic.o

.PHONY: check
check: chttp
	-$(MAKE) -C tests $@

.PHONY: clean
clean:
	-rm -f chttp *.o
	-$(MAKE) -C tests $@

