// SPDX-License-Identifier: GPL-2.0
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/string.h>
#include <linux/slab.h>

static int cmdline_proc_show(struct seq_file *m, void *v)
{
	char *buf, *p, *end;

	buf = kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	strscpy(buf, saved_command_line, PAGE_SIZE);

	/* Spoof verified boot state to green */
	p = strstr(buf, "androidboot.verifiedbootstate=");
	if (p) {
		p += strlen("androidboot.verifiedbootstate=");
		end = p;
		while (*end && *end != ' ' && *end != '\n')
			end++;
		/* overwrite with "green" */
		memcpy(p, "green", 5);
		if (end - p > 5)
			memset(p + 5, 0, end - p - 5);
	}

	seq_printf(m, "%s\n", buf);
	kfree(buf);
	return 0;
}

static int cmdline_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, cmdline_proc_show, NULL);
}

static const struct file_operations cmdline_proc_fops = {
	.open		= cmdline_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init proc_cmdline_init(void)
{
	proc_create("cmdline", 0, NULL, &cmdline_proc_fops);
	return 0;
}
fs_initcall(proc_cmdline_init);
