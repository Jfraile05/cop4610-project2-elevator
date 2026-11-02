obj-m += elevator.o
elevator-y := src/elevator_main.o src/elevator_sched.o src/elevator_proc.o

KDIR ?= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(CURDIR) modules

clean:
	make -C $(KDIR) M=$(CURDIR) clean
