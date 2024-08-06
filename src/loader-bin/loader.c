/* Standalone code to load the module */
#include <stdio.h>
#include <linux/module.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <unistd.h>

#include "../../artifacts/kshelf_loader.h"
#define MOD_NAME "kshelf_loader"

int init_module(void *module_image, unsigned long len, const char *param_values)
{
    return syscall(SYS_init_module, module_image, len, param_values);
}

int delete_module(const char *name, int flags)
{
    return syscall(SYS_delete_module, name, flags);
}


int main()
{
    char params[128];
    /* Read the taint value */
    char taint_buf[64];
    int taint_fd = open("/proc/sys/kernel/tainted", 0);
    if (read(taint_fd, taint_buf, sizeof(taint_buf)) < 0) {
        return -1;
    }
    close(taint_fd);
    snprintf(params, 128, "taint_value=%s", taint_buf);
    printf("%s", params);
    /* Read the console printk values */
    // int printk_fd = open("/proc/sys/kernel/printk", 0);
    /* Lower the console printk values*/

    /* Load + Unload the module */
    init_module(kshelf_loader, kshelf_loader_len, params);
    delete_module(MOD_NAME, 0);

    /* Restore the console printk */
}
