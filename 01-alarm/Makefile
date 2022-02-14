CFLAGS+=-g -Iinclude

CFILES:=$(wildcard src/*.c)
OFILES:=$(pathsubst src/%.c build/%.o $(CFILES))

.PHONY: all clean

all: build/alarmclock

build/alarmclock: $(OFILES)
	$(CC) $(LDFLAGS) $^ -o $@

build/%.o: src/%.c build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir build

clean:
	rm -rf build
