#include <linux/init.h> 
#include <linux/module.h>
#include <linux/kernel.h> 

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "AnnaUzh <anytik0509@gmail.com>" );
MODULE_DESCRIPTION("Its AnnaUzh's module !!!");

static int __init hello_init( void ) { 
   printk(KERN_ALERT "Hello, world!\n" ); 
   return 0; 
} 

static void __exit hello_exit( void ) { 
   printk(KERN_ALERT "Goodbye, world!" ); 
} 

module_init( hello_init ); 
module_exit( hello_exit ); 
