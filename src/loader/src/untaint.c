#include <linux/module.h>
#include <linux/kallsyms.h>
#include "include/untaint.h"
#include "include/util.h"

/* Remove taint flags from the kernel. */
void untaint_kernel(unsigned long value)
{
    unsigned long* tainted_mask = (unsigned long *)resolve_sym("tainted_mask");
    *tainted_mask = value;
}

#define TARGET_FUN "module_augment_kernel_taints"

void reset_already_done()
{
    int i = 0;
    unsigned long target = (unsigned long)resolve_sym(TARGET_FUN);
    unsigned long end = 0;
    unsigned long *rip_rel = 0;
    if (target == 0)
        return;

    char lookup[NAME_MAX];

    // find the boundaries
    for (i = 0; i < 0x10000; i+=16) {
        sprint_symbol(lookup, target + i);
        if (strstr(lookup, TARGET_FUN) > 0)
            continue;
        end = target + i;
        break;
    }

    // now we hunt for c6 05 xx xx xx xx 01, which should containn the offset to
    // a __already_done.
    // we only reset the last one
    
    for (unsigned char *p = (unsigned char *)target; p < (unsigned char *)end - 7; p++) {
        if ((*p) != 0xc6) continue;
        if (*(p + 1) != 0x05) continue;
        if (*(p + 6) != 0x01) continue;
        // we have an instance.
        unsigned int rel = *(unsigned int *)(p + 2);

        rip_rel = (unsigned long *)(p + 7 + rel);
    }

    *rip_rel = 0;
}
