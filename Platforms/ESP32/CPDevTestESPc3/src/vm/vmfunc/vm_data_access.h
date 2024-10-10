#ifndef _VM_DATA_ACCESS_

#define _VM_DATA_ACCESS_

#ifdef BYTE_ACCESS
#define _VM_ACCESS_FUNC_
#endif

#ifdef MEMCPY_ACCESS
#define _VM_ACCESS_FUNC_
#endif

#ifdef BIG_ENDIAN
#define _VM_ACCESS_FUNC_
#endif


/* data access functions */

#define getBOOL(x) (*(WM_BOOL*)(pgmData+x+wDataOfs))
#define setBOOL(x, val) getBOOL(x) = val

#define getBYTE(x) (*(WM_BYTE*)(pgmData+x+wDataOfs))
#define setBYTE(x, val) getBYTE(x) = val

#define getSINT(x) (*(WM_SINT*)(pgmData+x+wDataOfs))
#define setSINT(x, val) getSINT(x) = val

#ifndef _VM_ACCESS_FUNC_

#define getINT(x) (*(WM_INT*)(pgmData+x+wDataOfs))
#define setINT(x, val) getINT(x) = val

#define getDINT(x) (*(WM_DINT*)(pgmData+x+wDataOfs))
#define setDINT(x, val) getDINT(x) = val

#define getLINT(x) (*(WM_LINT*)(pgmData+x+wDataOfs))
#define setLINT(x, val) getLINT(x) = val

#define getREAL(x) (*(WM_REAL*)(pgmData+x+wDataOfs))
#define setREAL(x, val) getREAL(x) = val

#ifdef VM_LREAL_SUPPORT
#define getLREAL(x) (*(WM_LREAL*)(pgmData+x+wDataOfs))
#define setLREAL(x, val) getLREAL(x) = val
#endif

#define getADDRESS(x) (*(WM_ADDRESS*)(pgmData+x+wDataOfs))
#define setADDRESS(x, val) getADDRESS(x) = val

#define getWORD(x)	(*(WM_WORD*)(pgmData+x+wDataOfs))
#define setWORD(x, val) getWORD(x) = val

#define getDWORD(x) (*(WM_DWORD*)(pgmData+x+wDataOfs))
#define setDWORD(x, val) getDWORD(x) = val

#define getLWORD(x) (*(WM_LWORD*)(pgmData+x+wDataOfs))
#define setLWORD(x, val) getLWORD(x) = val

#else

WM_INT getINT(WM_ADDRESS x);
void setINT(WM_ADDRESS x, WM_INT val);

WM_DINT getDINT(WM_ADDRESS x);
void setDINT(WM_ADDRESS x, WM_DINT val);

#ifdef VM_LONG_SUPPORT
WM_LINT getLINT(WM_ADDRESS x);
void setLINT(WM_ADDRESS x, WM_LINT val);
#endif

WM_REAL getREAL(WM_ADDRESS x);
void setREAL(WM_ADDRESS x, WM_REAL val);

#ifdef VM_LREAL_SUPPORT
WM_LREAL getLREAL(WM_ADDRESS x);
void setLREAL(WM_ADDRESS x, WM_LREAL val);
#endif


WM_WORD getWORD(WM_ADDRESS x);
void setWORD(WM_ADDRESS x, WM_WORD val);

WM_DWORD getDWORD(WM_ADDRESS x);
void setDWORD(WM_ADDRESS x, WM_DWORD val);

#ifdef VM_LONG_SUPPORT
WM_LWORD getLWORD(WM_ADDRESS x);
void setLWORD(WM_ADDRESS x, WM_LWORD val);
#endif

#ifdef VM_ADDRESSING_32
#define getADDRESS getDWORD
#define setADDRESS setDWORD
#else
#define getADDRESS getWORD
#define setADDRESS setWORD
#endif

#endif

//#define getWORD	getINT
//#define setWORD setINT

//#define getDWORD getDINT
//#define setDWORD setDINT

//#define getLWORD getLINT
//#define setLWORD setLINT

#define getTIME(x)	getDWORD(x)
#define setTIME(x, val) setDWORD(x, val)

#ifdef VM_DATETIME_SUPPORT
#define getDATE(src,dst)	memcpy(&dst,(pgmData+src+wDataOfs),sizeof(WM_DATE))
#define setDATE(dst,src)	memcpy((pgmData+dst+wDataOfs),&src,sizeof(WM_DATE))

#define getTOD(src,dst)	memcpy(&dst,(pgmData+src+wDataOfs),sizeof(WM_TOD))
#define setTOD(dst,src)	memcpy((pgmData+dst+wDataOfs),&src,sizeof(WM_TOD))

#define getTIME_OF_DAY getTOD
#define setTIME_OF_DAY setTOD

#define getDATE_AND_TIME(src,dst)	memcpy(&dst,(pgmData+src+wDataOfs),sizeof(WM_DATE_AND_TIME))
#define setDATE_AND_TIME(dst,src)	memcpy((pgmData+dst+wDataOfs),&src,sizeof(WM_DATE_AND_TIME))
#endif

#ifdef VM_STRING_SUPPORT

void getSTRING(WM_ADDRESS src, WM_STRING* dst);  /* get string variable */
void setSTRING(WM_ADDRESS dst, WM_STRING* src);	/* set string variable */

#define getSTRING_DATA(x) (x.chars)

#endif

#define getDataPtr(x) (pgmData+x+wDataOfs)
#define getCodePtr(x) (pgmCode+x)

/* get a byte from code and advance program counter  */
#define GetProgramByte() (*getCodePtr(wProgramCounter++))

/* get a WORD from code */
WM_WORD GetProgramWord(void);

/* get a WORD from code */
WM_DWORD GetProgramDWord(void);

//#define GetProgramInt() (WM_INT)GetProgramWord()

/* get an address from code */
WM_ADDRESS GetProgramAddress(void);

#endif /* _VM_DATA_ACCESS_ */
