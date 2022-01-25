#ifdef VM_POINTER_SUPPORT

/* Program control instructions */
#ifdef FILE_DEBUG_INFO
#include <stdio.h>
void DebugFileCall(char* extraData, int mask);
char blkData[512];
#endif

void VMCLASS_PREFIX WMC_Pointers(WM_BYTE opt)
{

	switch(opt & 0xFF)
	{
		case VMF_DPSTO & 0xFF:		/* Stores current DPTR register in variable DST */
			{
				WM_ADDRESS r = GetProgramAddress();
				setADDRESS(r, wDataOfs);
#ifdef FILE_DEBUG_INFO
				sprintf(blkData, "DPSTO programAddress:0x%X wDataOfs:0x%X\n", r, wDataOfs);
				DebugFileCall(blkData, 1);
#endif
				break;
			}

		case VMF_DPRDL & 0xFF:		/* Copies COUNT bytes from memory indicated by pointer SRC to local variable DST */
			{
				WM_ADDRESS wDst = GetProgramAddress();
				WM_ADDRESS wSrc = getADDRESS(GetProgramAddress());
				WM_WORD count = GetProgramWord();
#ifdef FILE_DEBUG_INFO
				sprintf(blkData, "DPRDL wDst:0x%X+wDataOfs:0x%X wSrc:0x%X count:0x%X\n", wDst, wDataOfs, wSrc, count);
				DebugFileCall(blkData, 1);
#endif
				memcpy(pgmData+wDataOfs+wDst, pgmData+wSrc, count);
			}
			break;

		case VMF_DPWRL & 0xFF:		/* Copies COUNT bytes from local variable SRC to memory indicated by pointer DST */
			{
				WM_ADDRESS wSrc = GetProgramAddress();
				WM_ADDRESS wDst = getADDRESS(GetProgramAddress());
				WM_WORD count = GetProgramWord();
#ifdef FILE_DEBUG_INFO
				sprintf(blkData, "DPWRL wSrc:0x%X+wDataOfs:0x%X wDst:0x%X count:0x%X\n", wSrc, wDataOfs, wDst, count);
				DebugFileCall(blkData, 1);
#endif
				memcpy(pgmData+wDst, pgmData+wDataOfs+wSrc, count);
			}
			break;

		case VMF_CALBR & 0xFF:		/* User function block call by reference. Push old DPTR and CSIP values to separate stacks, set DPTR with InstPtr value*/
			{
				WM_ADDRESS wInstPtr = getADDRESS(GetProgramAddress());	/* Pointer to function block instance */
				WM_ADDRESS wFunBlockAdr = GetProgramAddress();			/* Address of function block code */
#ifdef FILE_DEBUG_INFO				
				sprintf(blkData, "CALBR instPtr:0x%X funBlkAddr:0x%X oldDataOfs:0x%X oldPC:0x%X\n", wInstPtr, wFunBlockAdr, wDataOfs, wProgramCounter);
				DebugFileCall(blkData, 1);
#endif
				push_DataOfsStack(wDataOfs);
				push_CallingStack(wProgramCounter);
				wDataOfs = wInstPtr;
				wProgramCounter = wFunBlockAdr;
			}
			break;

		case VMF_JNUL & 0xFF:		/* Conditional JUMP if the value is equal to NULL */
			{
				WM_ADDRESS par1 = getADDRESS(GetProgramAddress());	/* Pointer to addressing variable */
				WM_ADDRESS par2 = GetProgramAddress();				/* Address of code to jump */
#ifdef FILE_DEBUG_INFO
				sprintf(blkData, "JNUL par1:0x%X par2:0x%X\n", par1, par2);
				DebugFileCall(blkData, 1);
#endif
				if (!par1)
					wProgramCounter = par2;
			}
			break;

		case VMF_MEMCPP & 0xFF:		/* Memory copying function */
			{
				WM_ADDRESS pSourceAddress = getADDRESS(GetProgramAddress());
				WM_ADDRESS pDestinationAddress = getADDRESS(GetProgramAddress());
				WM_WORD BytesToCopy = GetProgramWord();
#ifdef FILE_DEBUG_INFO
				sprintf(blkData, "MEMCPP wSrc:0x%X wDst:0x%X count:0x%X\n", pSourceAddress, pDestinationAddress, BytesToCopy);
				DebugFileCall(blkData, 1);
#endif
				memcpy(pgmData+pDestinationAddress, pgmData+pSourceAddress, BytesToCopy);
			}
			break;
		default:
			bResult = WM_UNKNOWN;
			break;
	}
}

#endif