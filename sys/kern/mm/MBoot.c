#include "MBoot.h"
#include <preinit/lib/debug.h>

#define adr_low 536870912

//unsigned int FlatMem_LOC[adr_low];

unsigned int
fload(unsigned int addr)
{
    return *((unsigned int *) (addr * 4));
    //return FlatMem_LOC[addr];
}

void
fstore(unsigned int addr, unsigned int val)
{
    *((unsigned int *) (addr * 4)) = val;
    //FlatMem_LOC[addr] = val;
}