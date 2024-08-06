#include <linux/kprobes.h>

#include "./include/util.h"

/* the classic kprobe trick to find kallsyms_lookup_name, which we need on
 * modern kernels to implement our loader, as we need to use it for linking the
 * kSHELF */
static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};

unsigned long resolve_sym(const char *sym_name)
{
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
    unregister_kprobe(&kp);
    return kallsyms_lookup_name(sym_name);
}
