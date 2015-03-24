#include <preinit/lib/gcc.h>
#include <preinit/lib/debug.h>

#include "MALOp.h"

#define PAGESIZE 4096
#define NUM_PROC 64
#define PT_PERM_UP 0
#define PT_PERM_PTU 7

static char * PTPool_LOC[NUM_PROC][1024] gcc_aligned(PAGESIZE);
static unsigned int IDPMap_LOC[1024][1024] gcc_aligned(PAGESIZE);

void
pt_in(void)
{
}

void
pt_out(void)
{
}

static unsigned int current_pt = 99999;

void
set_pt(unsigned int index)
{
	current_pt = index;
	set_cr3(PTPool_LOC[index]);
}

unsigned int gcc_inline
get_pt ()
{
    return current_pt;
}

unsigned int
get_PDE(unsigned int proc_index, unsigned int pde_index)
{
    unsigned int pde;
    pde = (unsigned int)PTPool_LOC[proc_index][pde_index];
    return pde;
}   

void
set_PDE(unsigned int proc_index, unsigned int pde_index)
{   
    PTPool_LOC[proc_index][pde_index] = ((char *)(IDPMap_LOC[pde_index])) + PT_PERM_PTU;
}   

void
rmv_PDE(unsigned int proc_index, unsigned int pde_index)
{
    PTPool_LOC[proc_index][pde_index] = (char *)PT_PERM_UP;
}   

void
set_PDEU(unsigned int proc_index, unsigned int pde_index, unsigned int pi)
{
    PTPool_LOC[proc_index][pde_index] = (char *)(pi * 4096 + PT_PERM_PTU);
}   

unsigned int
get_PTE(unsigned int proc_index, unsigned int pde_index, unsigned int vadr)
{   
    unsigned int pte;
    unsigned int offset;
    offset = ((unsigned int)PTPool_LOC[proc_index][pde_index] - PT_PERM_PTU) / 4096;
    pte = fload(offset * 1024 + vadr);
    return pte;
}

void
set_PTE(unsigned int proc_index, unsigned int pde_index, unsigned int vadr, unsigned int padr, unsigned int perm)
{   
    unsigned int offset;
    offset = ((unsigned int)PTPool_LOC[proc_index][pde_index] - PT_PERM_PTU) / 4096;
    fstore(offset * 1024 + vadr, padr * 4096 + perm);
}   

void
rmv_PTE(unsigned int proc_index, unsigned int pde_index, unsigned int vadr)
{
    unsigned int offset;
    offset = ((unsigned int)PTPool_LOC[proc_index][pde_index] - PT_PERM_PTU) / 4096;
    fstore(offset * 1024 + vadr, 0);
}

void
set_IDPTE(unsigned int pde_index, unsigned int vadr, unsigned int perm)
{
    IDPMap_LOC[pde_index][vadr] = (pde_index * 1024 + vadr) * 4096 + perm;
}
