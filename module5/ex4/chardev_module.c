#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "AnnaUzh <anytik0509@gmail.com>" );
MODULE_DESCRIPTION("Its AnnaUzh's module !!!");

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUF_LEN 80

static int Major;
static int Device_Open = 0;
static char msg[BUF_LEN];
static char user_msg[BUF_LEN];
static char *msg_Ptr;
static int show_user_msg = 0;

static struct file_operations fops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};

int init_module(void)
{
  Major = register_chrdev(0, DEVICE_NAME, &fops);

  if (Major < 0) {    
    printk("Registering the character device failed with %d\n",
            Major);
    return Major;
  }

  printk(KERN_INFO "I was assigned major number %d.  To talk to\n", Major);

  return 0;
}

void cleanup_module(void)
{
  unregister_chrdev(Major, DEVICE_NAME);
}


static int device_open(struct inode *inode, struct file *file)
{
  static int counter = 0;
  if (Device_Open)
    return -EBUSY;
  Device_Open++;
  sprintf(msg, "I already told you %d times Hello world!\n", counter++);
  msg_Ptr = msg;
  try_module_get(THIS_MODULE);

  return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
  Device_Open--;
  module_put(THIS_MODULE);

  return 0;
}


static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset)
{
  int bytes_read = 0;
  if (show_user_msg == 1){
    show_user_msg = 0;
    msg_Ptr = user_msg;
  }
  if (*msg_Ptr == 0)
    return 0;

  while (length && *msg_Ptr) {
    put_user(*(msg_Ptr++), buffer++);
    length--;
    bytes_read++;
  }
  return bytes_read;
}

static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
  short ind = len - 1;
  short count = 0;
  memset(user_msg, 0, BUF_LEN);
  while (len > 0){
    user_msg[count++] = buff[ind--];
    len--;
  }
  user_msg[count++] = '\n';
  user_msg[count++] = '\0';
  return count;
}