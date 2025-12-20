#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define MAX_MSG_SIZE 10
#define PROC_FILENAME "proc"

static struct proc_dir_entry *our_proc_file;

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "AnnaUzh <anytik0509@gmail.com>" );
MODULE_DESCRIPTION("Its AnnaUzh's module !!!");

int len,temp;
char *msg;


static ssize_t read_proc(struct file *filp, char *buffer,  size_t length, loff_t * offset)
{
    if(length > temp) {
        length = temp;
    }
    temp = temp - length;
    copy_to_user(buffer, msg, length);
    if(length == 0)
        temp = len;
    return length;
}

static ssize_t write_proc(struct file *filp, const char *buff, size_t count, loff_t * off)
{
    copy_from_user(msg, buff, count);
    len = count;
    temp = len;
    return count;
}

static const struct file_operations proc_fops = {
    .read = read_proc,
    .write = write_proc,
};

static int __init proc_init(void)
{
    our_proc_file = proc_create(PROC_FILENAME, 0, NULL, &proc_fops);
    if (NULL == our_proc_file) {
        return -ENOMEM;
    }
    msg = kmalloc(MAX_MSG_SIZE, GFP_KERNEL);

    return 0;
}

static void __exit proc_cleanup(void)
{
  proc_remove(our_proc_file);
}

module_init(proc_init);
module_exit(proc_cleanup);