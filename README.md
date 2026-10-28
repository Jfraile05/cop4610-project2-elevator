# Elevator

A Linux kernel project: strace-verified system calls (Part 1), a `/proc/timer` kernel module
(Part 2), and a pet elevator implemented as three new system calls plus a kernel module with its
own scheduler thread (Part 3).

## Group Members
- **Jorge Fraile**: jfraile05@gmail.com
- **Ryan Baiano**: rb22br@fsu.edu
- **William Brandon**: wmb22e@fsu.edu

## Division of Labor

### Part 1: System Call Tracing
- **Responsibilities**: `empty.c`, `part1.c` (five extra syscalls), `strace` traces.
- **Assigned to**: Jorge Fraile, Ryan Baiano

### Part 2: Timer Kernel Module
- **Responsibilities**: `my_timer` module and `/proc/timer`.
- **Assigned to**: Ryan Baiano, William Brandon

### Part 3a: Adding System Calls
- **Responsibilities**: `syscalls.c`, syscall table entries 548-550, `syscalls.h` prototypes.
- **Assigned to**: Jorge Fraile, William Brandon

### Part 3b: Kernel Compilation
- **Responsibilities**: Building and installing Linux 6.16.x with the new calls.
- **Assigned to**: Jorge Fraile, Ryan Baiano

### Part 3c: Threads
- **Responsibilities**: Elevator kthread (`elevator_sched.c`).
- **Assigned to**: Ryan Baiano, William Brandon

### Part 3d: Linked List
- **Responsibilities**: Per-floor and in-elevator pet lists using `linux/list.h`.
- **Assigned to**: Jorge Fraile, William Brandon

### Part 3e: Mutexes
- **Responsibilities**: `elev_lock` protecting all shared state.
- **Assigned to**: Ryan Baiano, William Brandon

### Part 3f: Scheduling Algorithm
- **Responsibilities**: SCAN scheduling with FIFO boarding.
- **Assigned to**: Jorge Fraile, Ryan Baiano, William Brandon

## File Listing
```
elevator/
├── Makefile                      builds parts 1, 2 and 3
├── README.md
├── part1/
│   ├── empty.c / empty.trace     empty program and its strace output
│   ├── part1.c / part1.trace     five extra syscalls (getpid, getppid, getuid, getgid, geteuid)
│   └── Makefile
├── part2/
│   ├── src/my_timer.c            /proc/timer module
│   └── Makefile                  produces my_timer.ko
└── part3/
    ├── syscalls.c                start_elevator, issue_request, stop_elevator (kernel side)
    ├── Makefile                  produces elevator.ko
    ├── src/
    │   ├── elevator.h            constants, structs, shared declarations
    │   ├── elevator_main.c       module init/exit and the syscall implementations
    │   ├── elevator_sched.c      kthread and SCAN scheduler
    │   └── elevator_proc.c       /proc/elevator output
    ├── kernel-patch/
    │   ├── apply_patch.sh        adds the three syscalls to a kernel source tree
    │   ├── config_certs.sh       disables signing certificates in .config
    │   └── Makefile              copied to <kernel>/syscalls/Makefile
    └── tests/                    provided syscall check and producer/consumer programs
```

## How to Compile & Execute

### Requirements
- A Linux VM (VirtualBox or CS Proxmox), `gcc`, `make`, `strace`, kernel build tools.
- Linux kernel 6.16.x source in `/usr/src/`.

### Part 1
```
cd part1 && make && make traces
```

### Part 2
```
cd part2 && make
sudo insmod my_timer.ko
cat /proc/timer; sleep 1; cat /proc/timer
sudo rmmod my_timer
```

### Part 3
1. Patch and build the kernel (long: start early).
```
cd part3/kernel-patch
./apply_patch.sh /usr/src/linux-6.16.x
cd /usr/src/linux-6.16.x
cp /boot/config-$(uname -r) .config
~/<repo>/part3/kernel-patch/config_certs.sh /usr/src/linux-6.16.x
make menuconfig            # optional
make -j$(nproc)
sudo make modules_install
sudo make install
sudo reboot
uname -r                   # confirm the new kernel is running
```
2. Check the syscalls: `cd part3/tests/system-calls-test && make && sudo insmod syscheck.ko && ./test-syscalls`
3. Build and load the elevator:
```
cd part3 && make
sudo insmod elevator.ko
cd tests/elevator-test && make
./consumer --start
./producer 20
watch -n1 cat /proc/elevator
./consumer --stop
sudo rmmod elevator
```

## Design Notes
- **Syscalls**: `syscalls.c` defines function-pointer stubs (`STUB_*`) and the three
  `SYSCALL_DEFINE`s. `elevator.ko` fills the stubs on load and clears them on unload, so the calls
  return `-ENOSYS` when the module is not loaded.
- **Thread**: one kthread is created at `insmod` and sleeps while the elevator is OFFLINE.
  `start_elevator` wakes it; `stop_elevator` sets a flag, and the thread goes OFFLINE once the
  last rider is delivered. Waiting pets stay queued and are served after the next start.
- **Locking**: a single mutex (`elev_lock`) guards every shared field. The thread sleeps
  (2 s moves, 1 s loading) with the lock released so `/proc` reads and syscalls never block long.
- **Scheduling (SCAN)**: keep going in the current direction while a rider destination (or, if
  there is room, a waiting pet) lies ahead; otherwise reverse. At a stop, riders get off first,
  then waiting pets heading in the travel direction board in FIFO order. Boarding stops at the
  first eligible pet that does not fit, so FIFO is never violated. No pet starves because the
  elevator always sweeps the whole building.
- **Memory**: pets are `kmalloc`ed in `issue_request` and freed on arrival or on `rmmod`.
- `issue_request` returns 1 for floors outside 1-5, equal start/destination floors, or a bad
  type. Requests are accepted (queued) even while the elevator is OFFLINE.

## Known Bugs / Unfinished
- To be filled in after testing on the team's VM. The code was compiled against Linux 6.16
  headers and the scheduler was exercised in a user-space simulation, but it has not yet been
  booted in a custom kernel.
- Unloading the module while a syscall is executing inside the module is not guarded
  (same as the course example pattern).

## Development Log
- **Jorge Fraile**: <fill in dates and work>
- **Ryan Baiano**: <fill in dates and work>
- **William Brandon**: <fill in dates and work>

## Group Meetings
<fill in dates, attendees, and what was decided>

## Extra Credit / Special Considerations
None.
