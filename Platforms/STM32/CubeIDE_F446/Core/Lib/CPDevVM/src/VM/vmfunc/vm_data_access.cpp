#include "..\vm.h"
#include "vm_data_access.h"
#include "string.h"

/* Data access implementation functions */

#if defined (MEMCPY_ACCESS)

#define GETPROGRAM_MEMCPY(type) \
	WM_##type wRes; \
	memcpy(&wRes, getCodePtr(wProgramCounter), sizeof(WM_##type)); \
	wProgramCounter += sizeof(WM_##type); \
	return wRes;

#define GETDATA_MEMCPY(x, type) \
	WM_##type wRes; \
	memcpy(&wRes, pgmData+x+wDataOfs, sizeof(WM_##type)); \
	return wRes;

#define SETDATA_MEMCPY(x, val, type) \
	memcpy((pgmData + x + wDataOfs), &val, sizeof(WM_##type));

#endif

/* get a WORD from code */
WM_WORD VMCLASS_PREFIX GetProgramWord(void)
{

#if defined(BIG_ENDIAN)

	WM_WORD hi = GetProgramByte();
	WM_WORD lo = GetProgramByte();
	return (hi<<8) | lo;	/* little endian conversion */

#elif defined (BYTE_ACCESS)

	WM_WORD lo = GetProgramByte();
	WM_WORD hi = GetProgramByte();
	return (hi<<8) | lo;

#elif defined (MEMCPY_ACCESS)

	GETPROGRAM_MEMCPY(WORD);

#else

	WM_WORD res = (*(WM_WORD*)(getCodePtr(wProgramCounter)));
	wProgramCounter+=sizeof(WM_WORD);
	return res;

#endif
}

/* get a DWORD from code */

WM_DWORD VMCLASS_PREFIX GetProgramDWord(void)
{
#if defined (BYTE_ACCESS)

	WM_DWORD lo = GetProgramWord();
	WM_DWORD hi = GetProgramWord();
	return (hi<<16) | lo;

#elif defined (MEMCPY_ACCESS)

	GETPROGRAM_MEMCPY(DWORD);

#else

	WM_DWORD res = (*(WM_DWORD*)(getCodePtr(wProgramCounter)));
	wProgramCounter+=sizeof(WM_DWORD);
	return res;
#endif
}


/* get an address from code */

#ifdef VM_ADDRESSING_32

WM_ADDRESS VMCLASS_PREFIX GetProgramAddress(void)
{
#if defined (BYTE_ACCESS)

	WM_DWORD lo = GetProgramWord();
	WM_DWORD hi = GetProgramWord();
	return (hi<<16) | lo;

#elif defined (MEMCPY_ACCESS)

	GETPROGRAM_MEMCPY(ADDRESS);

#else

	WM_ADDRESS res = (*(WM_ADDRESS*)(getCodePtr(wProgramCounter)));
	wProgramCounter+=sizeof(WM_ADDRESS);
	return res;
#endif

}

#else

WM_ADDRESS VMCLASS_PREFIX GetProgramAddress(void)
{
	return GetProgramWord();
}

#endif

/* get or set a data value */

#ifdef BYTE_ACCESS

#include <string.h>

WM_REAL VMCLASS_PREFIX getREAL(WM_ADDRESS x)
{
	WM_REAL result;
	memcpy(&result,(pgmData+x+wDataOfs),sizeof(WM_REAL));
	return result;
}

void VMCLASS_PREFIX setREAL(WM_ADDRESS x, WM_REAL val)
{
	memcpy((pgmData+x+wDataOfs),&val,sizeof(WM_REAL));
}

#ifdef VM_LREAL_SUPPORT
WM_LREAL VMCLASS_PREFIX getLREAL(WM_ADDRESS x)
{
	WM_LREAL result;
	memcpy(&result,(pgmData+x+wDataOfs),sizeof(WM_LREAL));
	return result;
}

void VMCLASS_PREFIX setLREAL(WM_ADDRESS x, WM_LREAL val)
{
	memcpy((pgmData+x+wDataOfs),&val,sizeof(WM_LREAL));
}
#endif

WM_INT VMCLASS_PREFIX getINT(WM_ADDRESS x)
{
   WM_INT lo = (*(pgmData+x+wDataOfs));
   WM_INT hi = (*(pgmData+x+wDataOfs+1));
   return (hi<<8) | lo;
}

void VMCLASS_PREFIX setINT(WM_ADDRESS x, WM_INT val)
{
	(*(pgmData+x+wDataOfs)) = val&0xFF;
	(*(pgmData+x+wDataOfs+1)) = val>>8;
}

WM_DINT VMCLASS_PREFIX getDINT(WM_ADDRESS x)
{
	WM_DINT b1 = (*(pgmData+x+wDataOfs));
	WM_DINT b2 = (*(pgmData+x+wDataOfs+1));
	WM_DINT b3 = (*(pgmData+x+wDataOfs+2));
	WM_DINT b4 = (*(pgmData+x+wDataOfs+3));
	return (b4<<24)|(b3<<16)|(b2<<8)|b1;
}

void VMCLASS_PREFIX setDINT(WM_ADDRESS x, WM_DINT val)
{
	(*(pgmData+x+wDataOfs)) = (WM_BYTE)(val&0xFF);
	(*(pgmData+x+wDataOfs+1)) = (WM_BYTE)((val>>8)&0xFF);
	(*(pgmData+x+wDataOfs+2)) = (WM_BYTE)((val>>16)&0xFF);
	(*(pgmData+x+wDataOfs+3)) = (WM_BYTE)(val>>24);
}

WM_LINT VMCLASS_PREFIX getLINT(WM_ADDRESS x)
{
	WM_LINT b1 = getDINT(x);
	WM_LINT b2 = getDINT(x + 4);
	return (b2<<32)|b1;
}

void VMCLASS_PREFIX setLINT(WM_ADDRESS x, WM_LINT val)
{
	setDINT(x, (WM_DINT)(val & 0xFFFFFFFF));
	setDINT(x + 4, (WM_DINT)(val >> 32));
}

#elif defined (MEMCPY_ACCESS)

WM_REAL VMCLASS_PREFIX getREAL(WM_ADDRESS x)
{
	GETDATA_MEMCPY(x, REAL)
}

void VMCLASS_PREFIX setREAL(WM_ADDRESS x, WM_REAL val)
{
	SETDATA_MEMCPY(x, val, REAL)
}

WM_LREAL VMCLASS_PREFIX getLREAL(WM_ADDRESS x)
{
	GETDATA_MEMCPY(x, LREAL)
}

void VMCLASS_PREFIX setLREAL(WM_ADDRESS x, WM_LREAL val)
{
	SETDATA_MEMCPY(x, val, LREAL)
}

WM_INT VMCLASS_PREFIX getINT(WM_ADDRESS x)
{
	GETDATA_MEMCPY(x, INT)
}

void VMCLASS_PREFIX setINT(WM_ADDRESS x, WM_INT val)
{
	SETDATA_MEMCPY(x, val, INT)
}

WM_DINT VMCLASS_PREFIX getDINT(WM_ADDRESS x)
{
	GETDATA_MEMCPY(x, DINT)
}

void VMCLASS_PREFIX setDINT(WM_ADDRESS x, WM_DINT val)
{
	SETDATA_MEMCPY(x, val, DINT)
}

WM_LINT VMCLASS_PREFIX getLINT(WM_ADDRESS x)
{
	GETDATA_MEMCPY(x, LINT)
}

void VMCLASS_PREFIX setLINT(WM_ADDRESS x, WM_LINT val)
{
	SETDATA_MEMCPY(x, val, LINT)
}

#endif

#ifdef BIG_ENDIAN

WM_WORD ByteSwap2(WM_WORD nWordNumber)
{
   return ((nWordNumber >> 8) | (nWordNumber << 8));
}

WM_DWORD ByteSwap4(WM_DWORD nLongNumber)
{
   return (((nLongNumber&0x000000FF)<<24)+((nLongNumber&0x0000FF00)<<8)+
   ((nLongNumber&0x00FF0000)>>8)+((nLongNumber&0xFF000000)>>24));
}

void ByteSwap(unsigned char * b, int n)
{
   int i = 0;
   int j = n-1;
   unsigned char c;

   while (i<j)
   {
      c = b[i];
      b[i] = b[j];
      b[j] = c;
      i++, j--;
   }
}


WM_INT VMCLASS_PREFIX getINT(WM_ADDRESS x)
{
   WM_INT res = (*(WM_INT*)(pgmData+x+wDataOfs));
   return ByteSwap2(res);
}
void VMCLASS_PREFIX setINT(WM_ADDRESS x, WM_INT val)
{
	(*(WM_INT*)(pgmData+x+wDataOfs)) = ByteSwap2(val);
}

WM_DINT VMCLASS_PREFIX getDINT(WM_ADDRESS x)
{
   WM_DINT res = (*(WM_DINT*)(pgmData+x+wDataOfs));
   return ByteSwap4(res);
}
void VMCLASS_PREFIX setDINT(WM_ADDRESS x, WM_DINT val)
{
	(*(WM_DINT*)(pgmData+x+wDataOfs)) = ByteSwap4(val);
}

#endif

