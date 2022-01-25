
// default size of data area (pgmData)
#define DEFAULT_DATA_SIZE	1024

// size of the calling stack
#define CALL_STACK_SIZE         1024
  
// size of the data offset stack           
#define DATA_STACK_SIZE         1024             

// use byte-by-byte VM memory access to handle unaligned pointers
//#define BYTE_ACCESS

// use byte-by-byte VM memory access to handle unaligned pointers
#define MEMCPY_ACCESS

// use 32-bit code and data alignment
//#define ALIGN_4B

// compile VM as C++ class (instead of a set of C-functions)
#define VMTASK_CLASS

//enable support for STRING datatype
#define VM_STRING_SUPPORT

//enable support for LREAL datatype
#define VM_LREAL_SUPPORT

//enable support for POINTER datatype
#define VM_POINTER_SUPPORT

//enable support of functions EXPT,ABS,SQRT,LN,LOG,EXP,SIN,COS,TAN,ASIN,ACOS,ATAN,TRUNC,ROUND from math.h
#define VM_MATH_SUPPORT

//use 32-bit address size (16-bit is default)
//#define VM_ADDRESSING_32



#ifdef ARDUINO_AVR_UNO

// default size of data area (pgmData)
#undef DEFAULT_DATA_SIZE
#define DEFAULT_DATA_SIZE	128

// size of the calling stack
#undef CALL_STACK_SIZE
#define CALL_STACK_SIZE         128
  
// size of the data offset stack  
#undef DATA_STACK_SIZE        
#define DATA_STACK_SIZE         128 

// remove optional support to reduce code size
#undef VM_STRING_SUPPORT
#undef VM_LREAL_SUPPORT
#undef VM_MATH_SUPPORT

#endif
