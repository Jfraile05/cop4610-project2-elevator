#!/bin/bash
# Usage: ./apply_patch.sh /usr/src/linux-6.16.x
# Adds the three elevator system calls (548, 549, 550) to a kernel source tree.
# Safe to run more than once.
set -e

KDIR="${1:?usage: $0 <kernel source dir>}"
HERE="$(cd "$(dirname "$0")" && pwd)"
TBL="$KDIR/arch/x86/entry/syscalls/syscall_64.tbl"

[ -f "$TBL" ] || { echo "not a kernel tree: $KDIR"; exit 1; }

# 1. syscalls/ directory with the implementation and its Makefile
mkdir -p "$KDIR/syscalls"
cp "$HERE/../syscalls.c" "$KDIR/syscalls/syscalls.c"
cp "$HERE/Makefile" "$KDIR/syscalls/Makefile"

# 2. syscall table entries (548-550 are free, after the x32 block ending at 547)
if ! grep -q start_elevator "$TBL"; then
	printf '548\tcommon\tstart_elevator\t\tsys_start_elevator\n' >> "$TBL"
	printf '549\tcommon\tissue_request\t\tsys_issue_request\n' >> "$TBL"
	printf '550\tcommon\tstop_elevator\t\tsys_stop_elevator\n' >> "$TBL"
fi

# 3. prototypes, placed just before the closing #endif of syscalls.h
HDR="$KDIR/include/linux/syscalls.h"
if ! grep -q sys_start_elevator "$HDR"; then
	LINE=$(grep -n '^#endif' "$HDR" | tail -1 | cut -d: -f1)
	sed -i "${LINE}i\\
asmlinkage long sys_start_elevator(void);\\
asmlinkage long sys_issue_request(int start_floor, int destination_floor, int type);\\
asmlinkage long sys_stop_elevator(void);\\
" "$HDR"
fi

# 4. build the syscalls/ directory as part of the kernel.
# Kernels 6.x list top-level directories in Kbuild; older ones use core-y in Makefile.
if [ -f "$KDIR/Kbuild" ] && grep -q '^obj-y.*+= kernel/' "$KDIR/Kbuild"; then
	if ! grep -q 'syscalls/' "$KDIR/Kbuild"; then
		sed -i 's|^\(obj-y[[:space:]]*+= kernel/\)$|\1\nobj-y\t\t\t+= syscalls/|' "$KDIR/Kbuild"
	fi
	grep -q 'syscalls/' "$KDIR/Kbuild" || { echo "Add 'obj-y += syscalls/' to $KDIR/Kbuild by hand"; exit 1; }
else
	if ! grep -q 'syscalls/' "$KDIR/Makefile"; then
		sed -i 's|^\(core-y[[:space:]]*+= kernel/.*\)$|\1 syscalls/|' "$KDIR/Makefile"
	fi
	grep -q 'syscalls/' "$KDIR/Makefile" || { echo "Add syscalls/ to core-y in $KDIR/Makefile by hand"; exit 1; }
fi

echo "Patched $KDIR. Next: ./config_certs.sh $KDIR, then build (see README)."
