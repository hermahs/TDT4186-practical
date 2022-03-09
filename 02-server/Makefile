CFLAGS+=-g -Iinclude

CFILES:=$(wildcard src/*.c)
OFILES:=$(patsubst src/%.c,build/%.o,$(CFILES))

.PHONY: all clean

all: build/server

build/server: $(OFILES)
	$(CC) $(LDFLAGS) $^ -o $@

build/%.o: src/%.c build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir build

clean:
	rm -rf build
