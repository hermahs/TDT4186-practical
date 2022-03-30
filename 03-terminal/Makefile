CFLAGS+=-g -Iinclude -pthread

CFILES:=$(wildcard src/*.c)
OFILES:=$(patsubst src/%.c,build/%.o,$(CFILES))

.PHONY: all clean

all: build/terminal

build/terminal: $(OFILES)
	$(CC) $(LDFLAGS) $^ -o $@ -g -Wall

build/%.o: src/%.c build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir build

clean:
	rm -rf build
