/* SPDX-License-Identifier: GPL-2.0 */
#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/types.h>

#define MIN_FLOOR 1
#define MAX_FLOOR 5
#define MAX_PETS 5
#define MAX_WEIGHT 50
#define MIN_PET_WEIGHT 3

#define MOVE_TIME_MS 2000
#define LOAD_TIME_MS 1000
#define IDLE_POLL_MS 50

#define DIR_UP 1
#define DIR_DOWN (-1)

enum pet_type {
	CHIHUAHUA = 0,
	PUG,
	PUGHUAHUA,
	DACHSHUND,
	NUM_PET_TYPES
};

enum elevator_state {
	OFFLINE = 0,
	IDLE,
	LOADING,
	UP,
	DOWN
};

/* One pet, either waiting on a floor or riding the elevator. */
struct pet {
	int type;
	int weight;
	int start;
	int dest;
	struct list_head list;
};

/* All shared elevator state. Every field is protected by elev_lock. */
struct elevator {
	enum elevator_state state;
	int dir;		/* DIR_UP or DIR_DOWN, last travel direction */
	int floor;
	int load;		/* total weight of riders in lbs */
	int num_riders;
	int num_waiting;
	int num_serviced;
	bool running;		/* start_elevator() has been called */
	bool accepting;		/* false once stop_elevator() is requested */
	bool stopping;		/* deactivation in progress */
	struct list_head riders;
	struct list_head waiting[MAX_FLOOR + 1];	/* index 0 unused */
};

extern struct elevator elev;
extern struct mutex elev_lock;
extern wait_queue_head_t elev_wq;

static const int pet_weights[NUM_PET_TYPES] = { 3, 14, 10, 16 };
static const char pet_letters[NUM_PET_TYPES] = { 'C', 'P', 'H', 'D' };

/* elevator_sched.c */
int elevator_thread_fn(void *data);
void elevator_reset_state(void);
void elevator_free_all_pets(void);

/* elevator_proc.c */
int elevator_proc_init(void);
void elevator_proc_exit(void);

#endif /* ELEVATOR_H */
