// SPDX-License-Identifier: GPL-2.0
/*
 * my_timer: exposes /proc/timer. Each read prints the current wall-clock
 * time and, when a previous read exists, the time elapsed since that read.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/timekeeping.h>
#include <linux/time64.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cop4610t");
MODULE_DESCRIPTION("Prints current and elapsed time via /proc/timer");

#define ENTRY_NAME "timer"
#define PERMS 0444

static struct proc_dir_entry *timer_entry;
static struct timespec64 last_time;
static bool have_last;

static int timer_show(struct seq_file *m, void *v)
{
	struct timespec64 now, elapsed;

	ktime_get_real_ts64(&now);
	seq_printf(m, "current time: %lld.%09ld\n", (long long)now.tv_sec, now.tv_nsec);

	if (have_last) {
		elapsed = timespec64_sub(now, last_time);
		seq_printf(m, "elapsed time: %lld.%09ld\n",
			   (long long)elapsed.tv_sec, elapsed.tv_nsec);
	}

	last_time = now;
	have_last = true;
	return 0;
}

static int timer_open(struct inode *inode, struct file *file)
{
	return single_open(file, timer_show, NULL);
}

static const struct proc_ops timer_fops = {
	.proc_open = timer_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

static int __init timer_init(void)
{
	have_last = false;
	timer_entry = proc_create(ENTRY_NAME, PERMS, NULL, &timer_fops);
	if (!timer_entry)
		return -ENOMEM;
	return 0;
}

static void __exit timer_exit(void)
{
	proc_remove(timer_entry);
}

module_init(timer_init);
module_exit(timer_exit);
