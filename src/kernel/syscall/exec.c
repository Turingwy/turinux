#include "types.h"
#include "proc.h"
#include "file.h"
#include "dirent.h"
#include "vmm.h"
#include "pmm.h"
#include "elf.h"
#include "proc.h"
#include "exec.h"
#include "string.h"
#include "memory.h"
#include "rwi.h"

#define MIN(a, b)       (a)>(b)?(b):(a)
int exec(char *path, char **argv)
{
    static int i;
    char name[NAMELEN];
    struct elf_hdr elf;
    struct inode *ip = namei(path, name);
    struct elf_proghdr ph;
    int off;
    uint32_t stackend = 0;
    pgd_t *pgd;
    if(!ip)
        return -1;
    ilock(ip);
    if(readi(ip, (char *)&elf, 0, sizeof(struct elf_hdr)) != sizeof(struct elf_hdr))
        goto bad;
    if(elf.magic != ELF_MAGIC)
        goto bad;
    // init a new pgd that only contains maps of kernel mode.
    pgd = setup_kpgd();
    for(int i = 0, off = elf.phoff; i < elf.phnum; i++, off+=sizeof(struct elf_proghdr))
    {
        if(readi(ip, (char *)&ph, off, sizeof(struct elf_proghdr)) != sizeof(struct elf_proghdr))
            goto bad;

        if(ph.type != ELF_PROG_LOAD)
            continue;
        if(ph.memsz < ph.filesz)
            goto bad;
        int pgsz = ph.filesz >> 12;
        uint8_t *buf;
        int m;
        int filesz = ph.filesz;
        buf = kpalloc();
        for(int i = 0; i <= pgsz; i++) {
            readi(ip, buf, ph.off+PAGE_SIZE*i, PAGE_SIZE);
            m = MIN(PAGE_SIZE, filesz);
            inituvm(pgd, ph.vaddr+PAGE_SIZE*i, (uint32_t)buf, PAGE_SIZE, m);
            filesz -= m;
            if(ph.vaddr+PAGE_SIZE*i > stackend)
               stackend = ph.vaddr+PAGE_SIZE*i+4096;
        }
        kpfree(buf);
    }
    iunlock(ip);
    iput(ip);
    char *ustack = kpalloc();
    if(!ustack)
        goto bad;
    memset(ustack, 0, PAGE_SIZE);
    map(pgd, stackend, V2P(ustack), PAGE_WRITE | PAGE_PRESENT | PAGE_USER);
    int argc;
    for(argc = 0; argc < MAXARGV && argv[argc]; argc++)
        ;
    if(argc >= MAXARGV)
        goto bad;
    char **new_argv = (char **)kpalloc();
    char *sp = ustack+PAGE_SIZE;
       // add argv into the new user stack
    for(int i = argc-1; i >= 0; i--)
    {
        int len = strlen(argv[i])+1;
        sp-=len;
        strcpy(sp, argv[i]);
        new_argv[i] = (char *)((stackend & PAGE_MASK) + (sp-ustack));
    }
    sp -= sizeof(char *);
    *(uint32_t *)sp = 0;
    sp -= sizeof(char *) * argc;
    memcpy(sp, new_argv, sizeof(char *)*argc);
    kpfree((uint8_t *)new_argv);
    sp -= sizeof(char **);
    *(uint32_t *)sp = (stackend & PAGE_MASK) + (sp-ustack) + sizeof(char **);
    
    sp-=sizeof(int);
    *(uint32_t *)sp = argc;
    sp-=4;
    *(uint32_t *)sp = 0xffffffff;
    current_proc->regs->eip = elf.entry;
    current_proc->regs->useresp = (stackend & PAGE_MASK) + (sp-ustack);
    current_proc->pgd = pgd;
    switch_pgd(V2P(pgd));
    return 0; 
bad:
    if(ip) {
        iunlock(ip);
        iput(ip);
    }
    return -1;
            
       
}

