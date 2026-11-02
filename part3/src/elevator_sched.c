// SPDX-License-Identifier: GPL-2.0
/*
 * Elevator kthread and SCAN scheduling.
 *
 * The elevator keeps sweeping in its current direction while there is
 * anything ahead of it (a rider's destination or, if there is room, a
 * waiting pet). When nothing is ahead it reverses. At a stop riders get
 * off first, then pets board in strict FIFO order. Boarding stops at the
 * first pet that does not fit.
 *
 * All helpers below except elevator_thread_fn() require elev_lock held.
 */
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>

#include "elevator.h"

static bool can_accept_more(void)
{
	return elev.num_riders < MAX_PETS && elev.load + MIN_PET_WEIGHT <= MAX_WEIGHT;
}

static bool pet_fits(const struct pet *p)
{
	return elev.num_riders < MAX_PETS && elev.load + p->weight <= MAX_WEIGHT;
}

/* Number of riders whose destination is the current floor. */
static int count_unloads(void)
{
	struct pet *p;
	int n = 0;

	list_for_each_entry(p, &elev.riders, list)
		if (p->dest == elev.floor)
			n++;
	return n;
}

/* True if the queue head on this floor can board. */
static bool board_possible(void)
{
	struct pet *p;

	if (!elev.accepting)
		return false;

	if (list_empty(&elev.waiting[elev.floor]))
		return false;
	p = list_first_entry(&elev.waiting[elev.floor], struct pet, list);
	return pet_fits(p);
}

/* True if a rider destination or useful pickup lies beyond this floor. */
static bool has_ahead(int dir)
{
	struct pet *p;
	int f;

	list_for_each_entry(p, &elev.riders, list)
		if ((p->dest - elev.floor) * dir > 0)
			return true;

	if (!elev.accepting || !can_accept_more())
		return false;

	for (f = elev.floor + dir; f >= MIN_FLOOR && f <= MAX_FLOOR; f += dir)
		if (!list_empty(&elev.waiting[f]))
			return true;
	return false;
}

/* Direction the elevator will travel when it leaves (or serves) this floor. */
static int effective_dir(void)
{
	if (has_ahead(elev.dir) || board_possible())
		return elev.dir;
	return -elev.dir;
}

static bool stop_needed(void)
{
	return count_unloads() > 0 || board_possible();
}

/* Unload arriving riders, then board waiting pets in FIFO order. */
static void service_floor(void)
{
	struct pet *p, *tmp;

	list_for_each_entry_safe(p, tmp, &elev.riders, list) {
		if (p->dest != elev.floor)
			continue;
		list_del(&p->list);
		elev.load -= p->weight;
		elev.num_riders--;
		elev.num_serviced++;
		kfree(p);
	}

	elev.dir = effective_dir();
	if (!elev.accepting)
		return;

	list_for_each_entry_safe(p, tmp, &elev.waiting[elev.floor], list) {
		if (!pet_fits(p))
			break;
		list_move_tail(&p->list, &elev.riders);
		elev.load += p->weight;
		elev.num_riders++;
		elev.num_waiting--;
	}
}

/* Returns the direction to move, or 0 if there is nowhere to go. */
static int pick_move_dir(void)
{
	if (has_ahead(elev.dir))
		return elev.dir;
	if (has_ahead(-elev.dir))
		return -elev.dir;
	return 0;
}

void elevator_reset_state(void)
{
	elev.state = IDLE;
	elev.dir = DIR_UP;
	elev.floor = MIN_FLOOR;
	elev.load = 0;
	elev.num_riders = 0;
	elev.running = true;
	elev.accepting = true;
	elev.stopping = false;
}

/* Free every pet still on a floor or in the elevator (module exit only). */
void elevator_free_all_pets(void)
{
	struct pet *p, *tmp;
	int f;

	mutex_lock(&elev_lock);
	list_for_each_entry_safe(p, tmp, &elev.riders, list) {
		list_del(&p->list);
		kfree(p);
	}
	for (f = MIN_FLOOR; f <= MAX_FLOOR; f++) {
		list_for_each_entry_safe(p, tmp, &elev.waiting[f], list) {
			list_del(&p->list);
			kfree(p);
		}
	}
	elev.load = 0;
	elev.num_riders = 0;
	elev.num_waiting = 0;
	mutex_unlock(&elev_lock);
}

int elevator_thread_fn(void *data)
{
	int move;

	while (!kthread_should_stop()) {
		/* Sleep until start_elevator() is called. */
		wait_event_interruptible(elev_wq, kthread_should_stop() ||
					 READ_ONCE(elev.running));
		if (kthread_should_stop())
			break;

		mutex_lock(&elev_lock);
		if (!elev.running) {
			mutex_unlock(&elev_lock);
			continue;
		}

		/* stop_elevator() finishes once every rider has been delivered. */
		if (elev.stopping && elev.num_riders == 0) {
			elev.state = OFFLINE;
			elev.running = false;
			elev.stopping = false;
			mutex_unlock(&elev_lock);
			continue;
		}

		if (stop_needed()) {
			elev.state = LOADING;
			service_floor();
			mutex_unlock(&elev_lock);
			msleep(LOAD_TIME_MS);
			continue;
		}

		move = pick_move_dir();
		if (move == 0) {
			elev.state = IDLE;
			mutex_unlock(&elev_lock);
			msleep(IDLE_POLL_MS);
			continue;
		}

		elev.dir = move;
		elev.state = (move == DIR_UP) ? UP : DOWN;
		mutex_unlock(&elev_lock);

		msleep(MOVE_TIME_MS);

		mutex_lock(&elev_lock);
		elev.floor += move;
		mutex_unlock(&elev_lock);
	}
	return 0;
}
