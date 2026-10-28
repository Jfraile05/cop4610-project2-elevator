// SPDX-License-Identifier: GPL-2.0
/* /proc/elevator: prints the elevator and per-floor state. */
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include "elevator.h"

#define ENTRY_NAME "elevator"
#define PERMS 0444

static struct proc_dir_entry *elevator_entry;

static const char *state_name(enum elevator_state s)
{
	switch (s) {
	case OFFLINE:
		return "OFFLINE";
	case IDLE:
		return "IDLE";
	case LOADING:
		return "LOADING";
	case UP:
		return "UP";
	case DOWN:
		return "DOWN";
	}
	return "UNKNOWN";
}

static int elevator_show(struct seq_file *m, void *v)
{
	struct pet *p;
	int f;

	mutex_lock(&elev_lock);

	seq_printf(m, "Elevator state: %s\n", state_name(elev.state));
	seq_printf(m, "Current floor: %d\n", elev.floor);
	seq_printf(m, "Current load: %d lbs\n", elev.load);
	seq_puts(m, "Elevator status:");
	list_for_each_entry(p, &elev.riders, list)
		seq_printf(m, " %c%d", pet_letters[p->type], p->dest);
	seq_puts(m, "\n");

	for (f = MAX_FLOOR; f >= MIN_FLOOR; f--) {
		int count = 0;

		list_for_each_entry(p, &elev.waiting[f], list)
			count++;
		seq_printf(m, "[%c] Floor %d: %d", f == elev.floor ? '*' : ' ', f, count);
		list_for_each_entry(p, &elev.waiting[f], list)
			seq_printf(m, " %c%d", pet_letters[p->type], p->dest);
		seq_puts(m, "\n");
	}

	seq_printf(m, "Number of pets: %d\n", elev.num_riders);
	seq_printf(m, "Number of pets waiting: %d\n", elev.num_waiting);
	seq_printf(m, "Number of pets serviced: %d\n", elev.num_serviced);

	mutex_unlock(&elev_lock);
	return 0;
}

static int elevator_open(struct inode *inode, struct file *file)
{
	return single_open(file, elevator_show, NULL);
}

static const struct proc_ops elevator_fops = {
	.proc_open = elevator_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

int elevator_proc_init(void)
{
	elevator_entry = proc_create(ENTRY_NAME, PERMS, NULL, &elevator_fops);
	return elevator_entry ? 0 : -ENOMEM;
}

void elevator_proc_exit(void)
{
	proc_remove(elevator_entry);
}
