/* SHELF loader that supports several sections and basic relocations 
 * ifdefs so we can test stuff in userland and not kernel panic the box.
 */
#ifdef __KERNEL__
#include <linux/elf.h>
#include <linux/kprobes.h>
#include <linux/vmalloc.h>

#include "include/util.h"
#include "include/shelf.h"
#else
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <elf.h>

#define PAGE_SIZE 0x1000


#define printk printf
#define vmalloc malloc

#endif

typedef int *(*set_memory_x_t)(unsigned long addr, int numpages);
typedef int *(*set_memory_ro_t)(unsigned long addr, int numpages);

size_t get_n_pages(size_t n);
bool do_relocs(void *elf);
size_t get_virtualsize(void *elf);


#ifdef __KERNEL__
int strcmp(const char *s1, const char *s2);
/* Basic stolen strcmp implementation:
 * https://stackoverflow.com/questions/34873209/implementation-of-strcmp
 */
int strcmp(const char *s1, const char *s2)
{
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}
#endif


/* Maps a size to the number of pages */
size_t get_n_pages(size_t n)
{
    size_t i = (n / PAGE_SIZE);
    if ((n % PAGE_SIZE) > 0) {
        i += 1;
    }
    return i;
}


/* ELF LOADER */

bool do_relocs(void *elf)
{
    Elf64_Dyn *dyn = NULL;
    int dynamic_tags = 0;
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *) elf;
    for (uint16_t curr_ph = 0; curr_ph < ehdr->e_phnum; curr_ph++) {
        Elf64_Phdr *phdr = elf + ehdr->e_phoff  + curr_ph * ehdr->e_phentsize;
        if (phdr->p_type != PT_DYNAMIC) continue;
        dyn = elf + phdr->p_offset;
        dynamic_tags = phdr->p_filesz / sizeof(Elf64_Dyn); 
        break;
    }
    if (dyn == NULL) return true;
    /* Now we iterate through .dynamic looking for strtab, symtab, rela */
    Elf64_Rela *rela = NULL;
    Elf64_Sym *symtab = NULL;
    char *strtab = NULL;
    uint64_t relasz = 0;
    for (int i = 0; i < dynamic_tags; i++) {
        Elf64_Dyn *tag = &dyn[i];
        switch (tag->d_tag) {
            case DT_NULL:
                goto dt_end;

            case DT_RELA:
                rela = elf + tag->d_un.d_val;
                break;

            case DT_RELASZ:
                relasz = (uint64_t)tag->d_un.d_val;
                break;

            case DT_STRTAB:
                strtab = elf + tag->d_un.d_val;
                break;

            case DT_SYMTAB:
                symtab = elf + tag->d_un.d_val;
                break;
        }
    }
dt_end:
    if (rela == NULL || symtab == NULL || strtab == NULL)
        return false;

    relasz /= sizeof(Elf64_Rela);
    /* Now we iterate through the RELA */
    for (int i = 0; i < relasz; i++) {
        unsigned long *to_patch;
        int sym_idx = 0;
        char *symname = NULL;
        unsigned long sym_addr = 0;

        switch (ELF64_R_TYPE(rela[i].r_info)) {
            case R_X86_64_GLOB_DAT:
                /* symtab idx */
                sym_idx = ELF64_R_SYM(rela[i].r_info);
                symname = strtab + symtab[sym_idx].st_name;
#ifdef __KERNEL__
                sym_addr = resolve_sym(symname);
#endif
                printk("relocating sym: %s\n", symname);
                to_patch = \
                    (unsigned long *)(elf + rela[i].r_offset);
                *to_patch = sym_addr + rela[i].r_addend;
                break;
            case R_X86_64_RELATIVE:
                printk("relative relocation: %lli\n", rela[i].r_addend);
                to_patch = \
                    (unsigned long *)(elf + rela[i].r_offset);
                *to_patch = (unsigned long)elf + rela[i].r_addend;
                break;
            case R_X86_64_COPY:
                /* symtab idx */
                sym_idx = ELF64_R_SYM(rela[i].r_info);
                symname = strtab + symtab[sym_idx].st_name;
#ifdef __KERNEL__
                sym_addr = resolve_sym(symname);
#endif
                printk(
                    "copy sym: %s (%lli bytes)\n",
                    symname, symtab[sym_idx].st_size
                );
                to_patch = \
                    (unsigned long *)(elf + rela[i].r_offset);
                memcpy(
                    to_patch,
                    (void *) sym_addr + rela[i].r_addend,
                    symtab[sym_idx].st_size
                );
                break;
            default:
                printk("unknown relocation?\n");
                return false;
        }
    }

    return true;
}


/* Compute the size we actually need */
size_t get_virtualsize(void *elf)
{
    size_t res = 0;
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *) elf;
    for (uint16_t curr_ph = 0; curr_ph < ehdr->e_phnum; curr_ph++) {
        Elf64_Phdr *phdr = elf + ehdr->e_phoff  + curr_ph * ehdr->e_phentsize;
        if (phdr->p_type != PT_LOAD) continue;
        res += get_n_pages(phdr->p_memsz) * PAGE_SIZE;
    }
    return res;
}

/* process */
void run_elf(void *elf, size_t len)
{
    size_t size = get_virtualsize(elf);
    void *body = vmalloc(size);

    if (body == NULL) {
        printk("Unable to vmalloc memory?\n");
        return;
    }

    /* First copy the ELF to a new location */
    memset(body, 0, size);
    memcpy(body, elf, len);
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *) body;
    /* Apply the relocations by searching through the PHDRs for a PT_DYNAMIC */
    if (!do_relocs(body)) {
        return;
    }
    
    /* Go through the program headers to set correct page permissions for each
     * PT_LOAD */
    for (uint16_t curr_ph = 0; curr_ph < ehdr->e_phnum; curr_ph++) {
        Elf64_Phdr *phdr = body + ehdr->e_phoff  + curr_ph * ehdr->e_phentsize;
        if (phdr->p_type != PT_LOAD)
            continue;
        
        size_t size = get_n_pages(phdr->p_memsz);
        switch (phdr->p_flags & (PF_R | PF_W | PF_X)) {
            case PF_R | PF_W:
                /* Default case, nothing needs to be done */
                printk("RW\n");
                break;

            case PF_R | PF_X:
                printk("RX\n");
                /* Set RO, then make it executable */
#ifdef __KERNEL__
                set_memory_ro_t set_memory_ro = 
                    (set_memory_ro_t) resolve_sym("set_memory_ro");
                set_memory_x_t set_memory_x = 
                    (set_memory_x_t) resolve_sym("set_memory_x");
                set_memory_ro((uint64_t) body + phdr->p_vaddr, size);
                set_memory_x((uint64_t) body + phdr->p_vaddr, size);
#endif
                break;

            default:
                printk("Unsupported page permission\n");
                return;
        }
    }


    /* Transfer control */
    typedef void (*start_t)(void);
    printk("Entrypoint: %llx\n", (unsigned long) body + ehdr->e_entry);
    start_t start = (start_t)(body + ehdr->e_entry);

#ifdef __KERNEL__
    start();
#endif
}


#ifndef __KERNEL__
#include "../../artifacts/payload.h"
int main()
{
    run_elf(payload, payload_len);
}
#endif
