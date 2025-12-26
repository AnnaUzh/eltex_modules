#include <linux/module.h>
#include <linux/configfs.h>
#include <linux/init.h>
#include <linux/tty.h> 
#include <linux/kd.h>
#include <linux/vt.h>
#include <linux/console_struct.h>
#include <linux/vt_kern.h>
#include <linux/timer.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
 
MODULE_LICENSE("GPL");
struct timer_list my_timer;
struct tty_driver *my_driver;
static int _kbledstatus = 0;
static int test = 4;
#define BLINK_DELAY   HZ/5
#define ALL_LEDS_ON   0x07
#define RESTORE_LEDS  0xFF

static struct kobject *led_kobj;

static void __exit kbleds_cleanup(void)
{
        printk(KERN_INFO "kbleds: unloading...\n");
        del_timer(&my_timer);
        (my_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED,
                            RESTORE_LEDS);
        if (led_kobj) {
            kobject_put(led_kobj);
            led_kobj = NULL;
        }
}

static ssize_t test_show(struct kobject *kobj, 
                        struct kobj_attribute *attr, 
                        char *buf)
{
    return sprintf(buf, "%d\n", test);
}

static ssize_t test_store(struct kobject *kobj,
                         struct kobj_attribute *attr,
                         const char *buf,
                         size_t count)
{
   int new_value;
    int ret;
    
    ret = kstrtoint(buf, 10, &new_value);
    if (ret < 0) {
        printk(KERN_ERR "led: Not a number!\n");
        return ret;
    }
    
    if (new_value < 0 || new_value > 7) {
        printk(KERN_ERR "led: Value must be 0-7\n");
        return -EINVAL;
    }
    
    test = new_value;
    
    printk(KERN_INFO "led: test changed to %d\n", test);
    
    return count;
}

static struct kobj_attribute test_attr = __ATTR(test, 0644, test_show, test_store);


static void my_timer_func(struct timer_list *ptr)
{
        int *pstatus = &_kbledstatus;
        if (*pstatus == test)
                *pstatus = RESTORE_LEDS;
        else
                *pstatus = test;
        if (!my_driver || !my_driver->ops || !my_driver->ops->iostl) return;
        (my_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED,
                            *pstatus);
        my_timer.expires = jiffies + BLINK_DELAY;
        add_timer(&my_timer);
}
static int __init kbleds_init(void)
{
        int ret;
        printk(KERN_INFO "kbleds: loading\n");
        if (fg_console < 0 || fg_console > MAX_NR_CONSOLES){
            return -EINVAL;
        }

        my_driver = vc_cons[fg_console].d->port.tty->driver;
        printk(KERN_INFO "kbleds: tty driver magic %x\n", my_driver->magic);

        timer_setup(&my_timer, my_timer_func, 0);

        my_timer.expires = jiffies + BLINK_DELAY;
        add_timer(&my_timer);

        led_kobj = kobject_create_and_add("kbleds", &THIS_MODULE->mkobj.kobj);
        if (!led_kobj){
            printk(KER_ERR "led: Cannot create sysf object");
            del_timer(&my_timer);
            return -ENOMEM;
        }
        ret = sysfs_create_file(led_kobj, &test_attr.attr);
        if (ret){
            printk(KER_ERR "led: Cannot create sysf file");
            kobject_put(led_kobj);
            del_timer(&my_timer);
            return ret;
        }
        return 0;
}

module_init(kbleds_init);
module_exit(kbleds_cleanup);