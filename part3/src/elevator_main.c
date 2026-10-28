// SPDX-License-Identifier: GPL-2.0
/*
 * Elevator module: module init/exit and the bodies of the three system
 * calls. The system call stubs are defined in syscalls.c inside the kernel;
 * we point them at our implementations on load and clear them on unload.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/err.h>

#include "elevator.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cop4610t");
MODULE_DESCRIPTION("Pet elevator kernel module");

extern int (*STUB_start_elevator)(void);
extern int (*STUB_issue_request)(int, int, int);
extern int (*STUB_stop_elevator)(void);

struct elevator elev;
DEFINE_MUTEX(elev_lock);
DECLARE_WAIT_QUEUE_HEAD(elev_wq);

static struct task_struct *elevator_thread;

static int start_elevator(void)
{
	mutex_lock(&elev_lock);
	if (elev.running) {
		mutex_unlock(&elev_lock);
		return 1;
	}
	elevator_reset_state();
	mutex_unlock(&elev_lock);

	wake_up(&elev_wq);
	return 0;
}

static int issue_request(int start_floor, int destination_floor, int type)
{
	struct pet *p;

	if (start_floor < MIN_FLOOR || start_floor > MAX_FLOOR ||
	    destination_floor < MIN_FLOOR || destination_floor > MAX_FLOOR ||
	    start_floor == destination_floor ||
	    type < CHIHUAHUA || type >= NUM_PET_TYPES)
		return 1;

	p = kmalloc(sizeof(*p), GFP_KERNEL);
	if (!p)
		return -ENOMEM;

	p->type = type;
	p->weight = pet_weights[type];
	p->start = start_floor;
	p->dest = destination_floor;

	mutex_lock(&elev_lock);
	list_add_tail(&p->list, &elev.waiting[start_floor]);
	elev.num_waiting++;
	mutex_unlock(&elev_lock);
	return 0;
}

static int stop_elevator(void)
{
	int ret = 0;

	mutex_lock(&elev_lock);
	if (elev.stopping) {
		ret = 1;
	} else if (elev.running) {
		elev.stopping = true;
		elev.accepting = false;
	}
	mutex_unlock(&elev_lock);
	return ret;
}

static int __init elevator_init(void)
{
	int f, ret;

	elev.state = OFFLINE;
	elev.dir = DIR_UP;
	elev.floor = MIN_FLOOR;
	INIT_LIST_HEAD(&elev.riders);
	for (f = 0; f <= MAX_FLOOR; f++)
		INIT_LIST_HEAD(&elev.waiting[f]);

	ret = elevator_proc_init();
	if (ret)
		return ret;

	elevator_thread = kthread_run(elevator_thread_fn, NULL, "elevator");
	if (IS_ERR(elevator_thread)) {
		ret = PTR_ERR(elevator_thread);
		elevator_proc_exit();
		return ret;
	}

	STUB_start_elevator = start_elevator;
	STUB_issue_request = issue_request;
	STUB_stop_elevator = stop_elevator;
	return 0;
}

static void __exit elevator_exit(void)
{
	STUB_start_elevator = NULL;
	STUB_issue_request = NULL;
	STUB_stop_elevator = NULL;

	kthread_stop(elevator_thread);
	elevator_proc_exit();
	elevator_free_all_pets();
	mutex_destroy(&elev_lock);
}

module_init(elevator_init);
module_exit(elevator_exit);
