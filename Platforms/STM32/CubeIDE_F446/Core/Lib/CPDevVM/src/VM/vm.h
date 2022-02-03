#ifndef INC_VM_H
#define INC_VM_H

#include "vm_cfg.h"

#define VMCLASS_PREFIX


#ifdef __cplusplus

#ifdef VMTASK_CLASS

#undef  VMCLASS_PREFIX

#ifdef VM_ADDRESSING_32
#define VMCLASS_NAME VMTaskClass32
#else
#define VMCLASS_NAME VMTaskClass16
#endif

#define VMCLASS_PREFIX VMCLASS_NAME::

#endif

#endif

/* VM start mode constants */
#define WM_MODE_STOP    0x00
#define WM_MODE_NORMAL  0x01
#define WM_MODE_DEBUG   0x02
#define WM_MODE_EMULATION       0x04
#define WM_MODE_CONTINUE        0x08
#define WM_MODE_COLDRESTART     0x10
#define WM_MODE_PAUSED          0x20
#define WM_MODE_FIRST_START     0x40
#define WM_MODE_RUNNING			0x80

typedef unsigned char   WM_BOOL;                // change this, if the compiler supports bit data type
typedef unsigned char   WM_BYTE;

#if defined(__BORLANDC__)
#define long64 __int64
#else
#define long64 long long
#endif

                                               // dla AVR -> inaczej !!!!!!!!
typedef unsigned short  WM_WORD;
typedef unsigned long   WM_DWORD;
typedef unsigned long64      WM_LWORD;
typedef signed short    WM_INT;
typedef signed char             WM_SINT;
typedef signed long             WM_DINT;
typedef signed long64                WM_LINT;
typedef float                   WM_REAL;
#ifdef VM_LREAL_SUPPORT
typedef double                  WM_LREAL;
#endif

// aliasy
typedef WM_BYTE WM_USINT;
typedef WM_WORD WM_UINT;
typedef WM_DWORD WM_UDINT;
typedef WM_LWORD WM_ULINT;
typedef WM_DWORD WM_TIME;

// VM address size (16/32)
#define WM_ADDRESS16 WM_WORD
#define WM_ADDRESS32 WM_DWORD

#ifdef VM_ADDRESSING_32
#define WM_ADDRESS WM_ADDRESS32
#else
#define WM_ADDRESS WM_ADDRESS16
#endif

typedef union {
        struct {
                WM_BYTE day;
                WM_BYTE month;
                WM_WORD year;
        } date;
        WM_DWORD dw_date;
} WM_DATE;

typedef union {
        struct {
                WM_BYTE csec;
                WM_BYTE sec;
                WM_BYTE min;
                WM_BYTE hour;
        } tod;
        WM_DWORD dw_tod;
} WM_TIME_OF_DAY;

typedef WM_TIME_OF_DAY WM_TOD;

typedef struct {
                WM_TIME_OF_DAY tod;
                WM_DATE date;
} WM_DATE_AND_TIME;

#ifdef VM_STRING_SUPPORT

#ifdef VM_WIDECHAR
typedef wchar_t WM_CHARACTER;
#else
typedef char WM_CHARACTER;
#endif

typedef struct {
	WM_BYTE length;			// actual string length
	WM_BYTE chars_size;	// size of buffer chars 
	WM_WORD padding;		// reserved
	WM_CHARACTER* chars;	// string characters
} WM_STRING;

#endif


#if !defined(TRUE)
#define TRUE (1 == 1)
#define FALSE (1 == 0)
#endif // !defined(TRUE)

//#define DATA_SIZE                     1024    // size of the data area /* (obsolete) */
//#define BIG_ENDIAN                    /* for big endian compiler (obsolete) */

/* status word bit constants */
#define WMSTAT_CYCLEOVERRUN     0x0001
#define WMSTAT_ARRAYBOUND       0x0002
#define WMSTAT_COLDRESTART		0x0004
#define WMSTAT_FIRSTSTART		0x0008
#define WMSTAT_BADFORMAT		0x0010
#define WMSTAT_PAUSED			0x4000
#define WMSTAT_STOPPED			0x8000
#ifdef VMTASK_CLASS

#include "vmtaskclass.h"

#else

extern WM_BYTE *pgmCode;
extern WM_BYTE *pgmData;
extern WM_ADDRESS wProgramCounter;
extern WM_ADDRESS wDataOfs;
extern WM_WORD task_cycle;
extern WM_ADDRESS cycle_start_adr;
extern WM_WORD wStatus1;
extern WM_BYTE bRunMode;
extern unsigned long nCycles;

extern WM_ADDRESS wCallingStack[];		// function block calling stack
extern WM_BYTE wCallingStackPtr;		// and its pointer
extern WM_ADDRESS wDataOfsStack[];		// data offset stack
extern WM_BYTE wDataOfsStackPtr;		// and its pointer

/* general virtual machine functions */
int WM_Run(int mode);
int WM_Stop(void);

/* VM state direct control functions */
void WM_Initialize(int mode);
void WM_RunCycle();
void WM_Shutdown();

int WM_GetData(WM_ADDRESS index, WM_BYTE len, WM_BYTE* buf);
int WM_SetData(WM_ADDRESS index, WM_BYTE len, WM_BYTE* buf);

unsigned long WM_GetCycles(void);
WM_WORD WM_GetStatus1(void);


/* stack functions */
void push_DataOfsStack(WM_ADDRESS w);
WM_ADDRESS pull_DataOfsStack(void);
void push_CallingStack(WM_ADDRESS w);
WM_ADDRESS pull_CallingStack(void);

#endif

#include "vmplatform.h"

/* common funtions */
/* date and time functions */
int dow(int m, int d, int y);

WM_BOOL equDATE(WM_DATE d1, WM_DATE d2);
WM_BOOL lessDATE(WM_DATE d1, WM_DATE d2);

WM_BOOL equTOD(WM_TOD t1, WM_TOD t2);
WM_BOOL lessTOD(WM_TOD t1, WM_TOD t2);

WM_BOOL equDATE_AND_TIME(WM_DATE_AND_TIME dt1, WM_DATE_AND_TIME dt2);
WM_BOOL lessDATE_AND_TIME(WM_DATE_AND_TIME dt1, WM_DATE_AND_TIME dt2);

#ifdef VM_STRING_SUPPORT
WM_BOOL equSTRING(WM_STRING* s1, WM_STRING* s2);
WM_BOOL lessSTRING(WM_STRING* s1, WM_STRING* s2);
void copySTRING(WM_STRING* dst, WM_INT dst_offset, WM_STRING* src, WM_INT src_offset, WM_INT length);
#endif

/* debugging constants */
#define DBG_SET_CSIP 1
#define DBG_SET_REFPTR 2
#define DBG_SET_CLEAN_CSIP_STACK 3
#define DBG_SET_CLEAN_DPTR_STACK 4
#define DBG_VME_UNKNOWN_FUNCTION 5
#define DBG_VME_ARRAY_INDEX_BOUND 6
#define DBG_VME_STACK_OVERFLOW 7
#define DBG_VME_STACK_EMPTY 8
#define DBG_NOTIFY_TASK_CYCLE_START 9
#define DBG_VME_NATIVE_BLOCK 10

#endif /* INC_VM_H */
