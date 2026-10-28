CC = gcc

all: empty part1

empty: empty.c
	$(CC) -o empty empty.c

part1: part1.c
	$(CC) -o part1 part1.c

# Regenerate the submitted traces (requires strace).
traces: empty part1
	strace -o empty.trace ./empty
	strace -o part1.trace ./part1

.PHONY: all traces clean
clean:
	rm -f empty part1
