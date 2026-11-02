obj-m += syscheck.o

all: syscheck.ko test-syscalls

run:
	./test-syscalls

test_syscalls: test-syscalls.c test-syscalls.h
	gcc test-syscalls -o test_syscalls

syscheck.ko: syscheck.c
	make -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) modules

.PHONY: all clean run

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) clean
	rm -f test-syscalls