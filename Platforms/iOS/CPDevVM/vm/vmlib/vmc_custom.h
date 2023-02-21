/* Custom VM functions */

#include <math.h>
#define FINT_MAX 0x7FFFFFFFL
#define FINT_MIN (-FINT_MAX - 1L)
//#define FINT_DETECT_OVERRUNS

WM_BYTE WMC_Custom(WM_BYTE opcode);
