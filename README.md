# COP4610 Project 2: Pet Elevator

This repository contains the three project parts: system-call tracing, a `/proc/timer` kernel
module, and a pet elevator implemented with three new system calls and a kernel module.

## Team

- Jorge Fraile
- Ryan Baiano
- William Brandon

## Division of Labor

### Original plan

This is the division of labor written before the work began. It was marked as subject to
discussion and adjustment based on the team's agreement and individual expertise.

| Part | Team members |
|---|---|
| 1: System call tracing | Jorge Fraile, Ryan Baiano |
| 2: Timer kernel module | Ryan Baiano, William Brandon |
| 3a: Adding system calls | Jorge Fraile, William Brandon |
| 3b: Kernel compilation | Jorge Fraile, Ryan Baiano |
| 3c: Threads | Ryan Baiano, William Brandon |
| 3d: Linked list | Jorge Fraile, William Brandon |
| 3e: Mutexes | Ryan Baiano, William Brandon |
| 3f: Scheduling algorithm | Jorge Fraile, Ryan Baiano, William Brandon |

### Final division of labor

The final division is the same as the original plan. Each assignment below was performed by
the named members.

| Part | Responsibilities | Team members |
|---|---|---|
| 1 | Five system calls in `part1.c`, checked with `strace` | Jorge Fraile, Ryan Baiano |
| 2 | `my_timer` module and `/proc/timer` | Ryan Baiano, William Brandon |
| 3a | Add system calls 548-550 to the kernel | Jorge Fraile, William Brandon |
| 3b | Configure, compile and install the custom kernel | Jorge Fraile, Ryan Baiano |
| 3c | Elevator kernel thread | Ryan Baiano, William Brandon |
| 3d | Linked lists for waiting and riding pets | Jorge Fraile, William Brandon |
| 3e | Mutex protection of shared data | Ryan Baiano, William Brandon |
| 3f | Scheduling algorithm | Jorge Fraile, Ryan Baiano, William Brandon |

## Development Log

The team started the project on Oct 27 and worked on it through Nov 2. Day-by-day records
were not kept, so each stage is dated with the window the team worked in. Stages are
listed in the order the project was built, and every member's areas come from the
division of labor above.

### Team timeline

**Oct 27** - All three members met and started the project (see Meetings). The first version of parts 1 to 3 was committed to this repository that night.
**Oct 27 - Nov 2** - Parts 1 to 3 were built, run on the course VM (Linux 6.16.12) and refined 
**Nov 2** - Final verification on the course VM. The scheduler now boards in strict FIFO order, the module guards unloading against running system calls, and the README was completed.

### Jorge Fraile

1. **Oct 27: Kickoff.** Met with Ryan and William and started the project.
2. **Oct 27 to Nov 2: Part 1, system call tracing (with Ryan).** Five system calls in a
   test program and the `strace` traces. Files: `part1/`.
3. **Oct 27 to Nov 2: Part 3a, adding system calls (with William).** System calls 548 to
   550 added to the kernel. Files: `part3/syscalls.c`, `part3/kernel-patch/`.
4. **Oct 27 to Nov 2: Part 3b, kernel compilation (with Ryan).** Configured, built and
   installed the custom kernel. Steps: README, Part 3 step 1.
5. **Oct 27 to Nov 2: Part 3d, linked lists (with William).** Waiting and rider lists for
   pets. Files: `part3/src/elevator.h`, `part3/src/elevator_main.c`.
6. **Oct 27 to Nov 2: Part 3f, scheduling algorithm (with Ryan and William).** Elevator
   scheduling and FIFO boarding. File: `part3/src/elevator_sched.c`.

### Ryan Baiano

1. **Oct 27: Kickoff.** Met with Jorge and William and started the project.
2. **Oct 27 to Nov 2: Part 1, system call tracing (with Jorge).** Five system calls in a
   test program and the `strace` traces. Files: `part1/`.
3. **Oct 27 to Nov 2: Part 2, timer module (with William).** `my_timer` module and the
   `/proc/timer` entry. File: `part2/src/my_timer.c`.
4. **Oct 27 to Nov 2: Part 3b, kernel compilation (with Jorge).** Configured, built and
   installed the custom kernel. Steps: README, Part 3 step 1.
5. **Oct 27 to Nov 2: Parts 3c and 3e, threads and mutexes (with William).** Elevator
   kthread and `elev_lock` protection of shared data. Files: `part3/src/`.
6. **Oct 27 to Nov 2: Part 3f, scheduling algorithm (with Jorge and William).** Elevator
   scheduling and FIFO boarding. File: `part3/src/elevator_sched.c`.

### William Brandon

1. **Oct 27: Kickoff.** Met with Jorge and Ryan and started the project.
2. **Oct 27 to Nov 2: Part 2, timer module (with Ryan).** `my_timer` module and the
   `/proc/timer` entry. File: `part2/src/my_timer.c`.
3. **Oct 27 to Nov 2: Part 3a, adding system calls (with Jorge).** System calls 548 to 550
   added to the kernel. Files: `part3/syscalls.c`, `part3/kernel-patch/`.
4. **Oct 27 to Nov 2: Parts 3c and 3e, threads and mutexes (with Ryan).** Elevator kthread
   and `elev_lock` protection of shared data. Files: `part3/src/`.
5. **Oct 27 to Nov 2: Part 3d, linked lists (with Jorge).** Waiting and rider lists for
   pets. Files: `part3/src/elevator.h`, `part3/src/elevator_main.c`.
6. **Oct 27 to Nov 2: Part 3f, scheduling algorithm (with Jorge and Ryan).** Elevator
   scheduling and FIFO boarding. File: `part3/src/elevator_sched.c`.

## Meetings

### Oct 27

- **Attendees:** Jorge Fraile, Ryan Baiano, William Brandon.
- **Topics discussed:** The team worked together on all parts: system call tracing, the
  timer module, adding system calls, building the kernel, the elevator thread, linked
  lists, mutexes and the scheduling algorithm.
- **Outcomes:** Each member took the areas listed in the division of labor. By the end of
  the day the first version of parts 1 to 3 was committed to this repository.

## File Listing

```
part1/                      empty.c, part1.c, their strace traces, Makefile
part2/
  Makefile                  builds my_timer.ko
  src/my_timer.c            /proc/timer module
part3/
  Makefile                  builds elevator.ko
  syscalls.c                kernel-side system calls 548-550 (copied into the kernel tree)
  src/elevator.h            shared types, constants and declarations
  src/elevator_main.c       module init/exit and the bodies of the three system calls
  src/elevator_sched.c      elevator kthread and scheduling
  src/elevator_proc.c       /proc/elevator output
  kernel-patch/             apply_patch.sh and config_certs.sh for the kernel source tree
  tests/system-calls-test/  syscheck module and test-syscalls program
  tests/elevator-test/      producer, consumer and fifo programs
Makefile                    builds or cleans all parts
```

## Requirements

An x86-64 Linux VM with `gcc`, `make`, `strace`, kernel build dependencies, and Linux 6.16.x
source. Building a kernel takes a long time, so take a VM snapshot first.

## Part 1: System Call Tracing

```sh
cd part1
make            # builds empty and part1
make traces     # regenerates empty.trace and part1.trace
```

`part1.trace` has five more lines than `empty.trace` (getpid, getppid, getuid, getgid,
geteuid). The built `empty` and `part1` binaries are ignored by git.

## Part 2: Timer Module

```sh
cd part2
make
sudo insmod my_timer.ko
cat /proc/timer
sleep 1
cat /proc/timer        # prints current time and elapsed time since the previous read
sudo rmmod my_timer
```

## Part 3: Kernel and Elevator

### 1. Patch, build and install the kernel

Run from the repository root. Set `KDIR` to your actual Linux 6.16.x source directory.
`apply_patch.sh` copies `part3/syscalls.c` into `$KDIR/syscalls/`, adds syscalls 548-550 to
`syscall_64.tbl` and `syscalls.h`, and adds `syscalls/` to the kernel build. It is safe to
run again.

```sh
KDIR=/usr/src/linux-6.16.12
bash part3/kernel-patch/apply_patch.sh "$KDIR"
cp "/boot/config-$(uname -r)" "$KDIR/.config"
bash part3/kernel-patch/config_certs.sh "$KDIR"
make -C "$KDIR" -j"$(nproc)"
sudo make -C "$KDIR" modules_install
sudo make -C "$KDIR" install
sudo reboot
```

**Important:** the current `syscalls.c` also exports `elevator_stub_sem`, a read/write
semaphore that makes unloading the module safe while a system call is running. If the
installed kernel was built from an older `syscalls.c`, `elevator.ko` still loads (it treats
the semaphore as optional), but that protection is missing. For the full behavior, re-run
`apply_patch.sh`, rebuild, reinstall and reboot (an incremental rebuild after a one-file
change is much faster than the first build). Confirm the kernel with `uname -r`.

### 2. Check the system calls

```sh
cd part3/tests/system-calls-test
make
sudo insmod syscheck.ko
./test-syscalls
sudo rmmod syscheck
lsmod | grep syscheck        # must print nothing before loading the elevator
cd ../../..
```

`syscheck` and `elevator` both fill the same system-call hooks, so `syscheck` must be
removed before the elevator is loaded.

### 3. Build and load the elevator

```sh
cd part3
make
sudo insmod elevator.ko
cd tests/elevator-test
make
```

### 4. Run

```sh
# Terminal 1: start the elevator
./consumer --start

# Terminal 2: submit random requests
./producer 20

# Terminal 3: watch the status
watch -n1 cat /proc/elevator

# When the producer is done, Terminal 1:
./consumer --stop
```

After `--stop`, the elevator delivers everyone on board and then reports `OFFLINE`. Remove
the module only after `/proc/elevator` shows `OFFLINE`:

```sh
sudo rmmod elevator
```

Pets still waiting on floors when the elevator stops stay in their queues until it is
started again or the module is removed (they are freed on removal).

**About `./producer N`:** the provided producer picks floors 1 to 6, but the building has
floors 1 to 5. Requests involving floor 6 are rejected by `issue_request` (it returns 1 and
the producer prints `returned 1`). So `./producer 20` issues 20 requests, of which only
those printing `returned 0` are accepted and serviced.

### Controlled FIFO check

With the elevator `OFFLINE`, queue two requests on the same floor, then start it:

```sh
cd part3/tests/elevator-test
./fifo                  # older Chihuahua 3 to 1, then younger Pug 3 to 5
cat /proc/elevator      # Floor 3: 2 C1 P5
./consumer --start
watch -n1 cat /proc/elevator
```

The Chihuahua boards first; the Pug never boards ahead of it.

## Design Summary

- **System calls.** `part3/syscalls.c` defines the three system calls inside the kernel.
  Each one calls a function pointer (`STUB_start_elevator`, `STUB_issue_request`,
  `STUB_stop_elevator`) that the elevator module sets on load. With no module loaded they
  return `-ENOSYS`. The dispatch runs under a read lock of `elevator_stub_sem`, and
  `elevator_exit` takes the write lock before clearing the pointers, so no call can still
  be running module code when the module is unloaded (when the kernel provides the lock).
- **Threading and locking.** One kthread (`elevator`) controls all movement. A single
  mutex, `elev_lock`, protects every shared field (state, floor, load, counters, rider
  list, per-floor waiting lists). The thread releases the mutex while it sleeps for
  travel (2.0 s per floor) and loading (1.0 s).
- **Data structures.** Waiting pets (one `list_head` per floor) and riders are kernel
  linked lists. Each `struct pet` is allocated with `kmalloc` when the request is accepted
  and freed with `kfree` when delivered or when the module is removed.
- **Scheduling.** The elevator sweeps in its current direction while there is work ahead:
  a rider's destination or, if the elevator has room, a waiting pet. When nothing is ahead
  it reverses. At a stop, riders get off first, then pets board.
- **FIFO.** Boarding on a floor always takes the oldest waiting pet first, regardless of
  its direction. If that pet does not fit (5 pets or 50 lbs), nobody behind it boards. The
  elevator then carries that pet to its destination before the next one is considered.
- **Stopping.** `stop_elevator` stops new boarding, delivers current riders, then sets the
  state to `OFFLINE`. `start_elevator` returns 1 while the elevator is running or
  stopping. `issue_request` returns 1 for an invalid floor, identical floors, or an invalid
  type, and `-ENOMEM` if allocation fails.
- **Init failure.** If `/proc/elevator` or the kthread cannot be created, `elevator_init`
  undoes what it already set up and returns the error.

## Testing

- An earlier version of this project was run on an Ubuntu x86-64 VM with Linux 6.16.12:
  syscalls 548-550 were present, `syscheck` passed, `my_timer` printed an elapsed time of
  about 2.01 s after a 2 s sleep, and `./producer 20` serviced all 14 accepted requests
  (6 were rejected for floor 6) with the elevator ending `OFFLINE`.
- That run showed a FIFO problem: with a younger Pug (3 to 5) queued behind an older
  Chihuahua (3 to 1), the Pug boarded first. The scheduler now takes the oldest pet first.
- The current scheduler source was checked in a user-space harness that runs
  `elevator_sched.c` on simulated time, with 600 randomized request streams (including
  heavy bursts). It checked the 5-pet and 50-lb limits, one floor per move, strict
  per-floor FIFO, that the mutex is never held while sleeping, that all accepted pets
  were serviced, and that stop ends with an empty elevator in `OFFLINE`. The harness
  also failed when FIFO was deliberately broken, so it detects violations.
- The patched `syscalls.c`, `elevator.ko`, `my_timer.ko`, and `syscheck.ko` compile against
  an unmodified Linux 6.16.12 tree with the patch applied.
- The current code was then run end to end on the course VM, on Linux 6.16.12 built from
  the current `syscalls.c` (`elevator_stub_sem` is present in `/proc/kallsyms`). A fresh
  clone of this repository built with no warnings. Results:
  - Part 1: `part1.trace` has 36 lines and `empty.trace` has 31, so 5 more.
  - Part 2: `my_timer` loaded, `/proc/timer` was read twice, and it unloaded.
  - System calls: `syscheck` and `./test-syscalls` passed, and `syscheck` was removed
    before the elevator was loaded.
  - Elevator: `./fifo` and the controlled FIFO check passed, then a 3-minute series of
    `./producer 20` runs. Load reached 50 lbs and 5 pets but never went over either
    limit, and both UP and DOWN were seen.
  - Stop: `./consumer --stop` with riders aboard delivered them, and the state reached
    `OFFLINE` after about 11 seconds. `rmmod elevator` removed `/proc/elevator`.
  - `dmesg`: the only new lines were two notices from loading the out-of-tree `my_timer`
    module.

## Known Limitations

- The module must be removed only after the elevator is `OFFLINE`. Removing it while
  running stops the thread immediately and frees pets that have not been delivered.
- Strict FIFO means a pet that wants to go the opposite way can make the elevator carry
  it first, so total travel is not minimal.

## Bugs

- No known functional bugs at the time of writing. Not exercised on the VM: the
  `elevator_stub_sem` unload guard while a system call races with `rmmod`, and the
  one-floor-per-move rule, which was checked only in the scheduler simulation.
- The provided `producer` can request floor 6, which is rejected by design.

## Considerations

- Strict per-floor FIFO was chosen over a faster direction-aware pickup because the
  assignment says pets board in FIFO order.
- Removing `elevator` while it is running frees undelivered pets; stop it first.
- A stop request while OFFLINE has no effect and returns 0.
