// SPDX-License-Identifier: GPL-2.0
/*
 * Elevator system calls (548, 549, 550). Each call forwards to a function
 * pointer that the elevator module fills in when it is loaded. If the
 * module is not loaded the calls fail with -ENOSYS.
 */
#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rwsem.h>
#include <linux/syscalls.h>

DECLARE_RWSEM(elevator_stub_sem);
EXPORT_SYMBOL(elevator_stub_sem);

int (*STUB_start_elevator)(void) = NULL;
int (*STUB_issue_request)(int, int, int) = NULL;
int (*STUB_stop_elevator)(void) = NULL;
EXPORT_SYMBOL(STUB_start_elevator);
EXPORT_SYMBOL(STUB_issue_request);
EXPORT_SYMBOL(STUB_stop_elevator);

SYSCALL_DEFINE0(start_elevator)
{
	int ret;

	down_read(&elevator_stub_sem);
	ret = STUB_start_elevator ? STUB_start_elevator() : -ENOSYS;
	up_read(&elevator_stub_sem);
	return ret;
}

SYSCALL_DEFINE3(issue_request, int, start_floor, int, destination_floor, int, type)
{
	int ret;

	down_read(&elevator_stub_sem);
	ret = STUB_issue_request ?
		STUB_issue_request(start_floor, destination_floor, type) : -ENOSYS;
	up_read(&elevator_stub_sem);
	return ret;
}

SYSCALL_DEFINE0(stop_elevator)
{
	int ret;

	down_read(&elevator_stub_sem);
	ret = STUB_stop_elevator ? STUB_stop_elevator() : -ENOSYS;
	up_read(&elevator_stub_sem);
	return ret;
}
