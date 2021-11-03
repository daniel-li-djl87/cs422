#include <lib/elf.h>
#include <lib/debug.h>
#include <lib/gcc.h>
#include <lib/seg.h>
#include <lib/trap.h>
#include <lib/x86.h>
#include <lib/spinlock.h>
#include <pcpu/PCPUIntro/export.h>

#include "import.h"

extern tf_t uctx_pool[NUM_IDS];

extern unsigned int last_active[NUM_CPUS];
extern spinlock_t thread_lock;

void proc_start_user(void)
{
    unsigned int cur_pid = get_curid();
    unsigned int cpu_idx = get_pcpu_idx();

    kstack_switch(cur_pid);
    set_pdir_base(cur_pid);
    last_active[cpu_idx] = cur_pid;

    spinlock_release(&thread_lock);
    KERN_DEBUG("(start) released thread lock cpu %d pid %d\n", get_pcpu_idx(), get_curid());

    trap_return((void *) &uctx_pool[cur_pid]);
}

unsigned int proc_create(void *elf_addr, unsigned int quota)
{
    unsigned int pid, id;

    id = get_curid();
    pid = thread_spawn((void *) proc_start_user, id, quota);

    if (pid != NUM_IDS) {
        elf_load(elf_addr, pid);

        uctx_pool[pid].es = CPU_GDT_UDATA | 3;
        uctx_pool[pid].ds = CPU_GDT_UDATA | 3;
        uctx_pool[pid].cs = CPU_GDT_UCODE | 3;
        uctx_pool[pid].ss = CPU_GDT_UDATA | 3;
        uctx_pool[pid].esp = VM_USERHI;
        uctx_pool[pid].eflags = FL_IF;
        uctx_pool[pid].eip = elf_entry(elf_addr);

        seg_init_proc(get_pcpu_idx(), pid);
    }

    return pid;
}
