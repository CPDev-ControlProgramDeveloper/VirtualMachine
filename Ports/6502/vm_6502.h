#ifndef VM_6502_H
#define VM_6502_H

//#define USE_XCP_FILE

#include "vm/vm.h"

#ifndef USE_XCP_FILE
int VMP_LoadProgramFromArray(const unsigned char* code, int datasize);
#endif

#endif