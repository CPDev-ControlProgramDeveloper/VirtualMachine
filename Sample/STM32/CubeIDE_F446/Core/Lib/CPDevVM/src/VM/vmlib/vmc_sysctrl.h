/* Program control instructions */
#ifdef FILE_DEBUG_INFO
#include <stdio.h>
extern char blkData[512];
void DebugFileCall(char *, int);
#endif

void VMCLASS_PREFIX WMC_SYSCTRL(WM_BYTE opt)
{
	//	WM_WORD wPar1, wPar2;

	switch (opt & 0xFF)
	{
	case VMF_JMP & 0xFF: /* JMP - unconditional jump */
		wProgramCounter = GetProgramAddress();
		break;

	case VMF_JNZ & 0xFF:
	{
		WM_BOOL par1 = getBOOL(GetProgramAddress());
		WM_ADDRESS par2 = GetProgramAddress();
		if (par1)
			wProgramCounter = par2;
	}
	break;

	case VMF_JZ & 0xFF:
	{
		WM_BOOL par1 = getBOOL(GetProgramAddress());
		WM_ADDRESS par2 = GetProgramAddress();
		if (!par1)
			wProgramCounter = par2;
	}
	break;

	case VMF_JR & 0xFF:
	{
		WM_ADDRESS par1 = GetProgramAddress();
		wProgramCounter += par1;
	}
	break;

	case VMF_JRN & 0xFF:
	{
		WM_BOOL par1 = getBOOL(GetProgramAddress());
		WM_ADDRESS par2 = GetProgramAddress();
		if (par1)
			wProgramCounter += par2;
	}
	break;

	case VMF_JRZ & 0xFF:
	{
		WM_BOOL par1 = getBOOL(GetProgramAddress());
		WM_ADDRESS par2 = GetProgramAddress();
		if (!par1)
			wProgramCounter += par2;
	}
	break;

	case VMF_CALF & 0xFF: /* CALF - call a function */
	case VMF_CALB & 0xFF: /* CALB - call a function block */
	{
		WM_ADDRESS wPar1 = GetProgramAddress(); /* parameter area */
		WM_ADDRESS wPar2 = GetProgramAddress(); /* function code*/
#ifdef FILE_DEBUG_INFO
		sprintf(blkData, "CALB instPtr:0x%X funBlkAddr:0x%X oldDataOfs:0x%X oldPC:0x%X\n", wPar1, wPar2, wDataOfs, wProgramCounter);
		DebugFileCall(blkData, 1);
#endif
		push_DataOfsStack(wDataOfs); //-- by�o wDataOffsetReg
		push_CallingStack(wProgramCounter);
		wDataOfs += wPar1; //-- by�o wDataOffsetReg
		wProgramCounter = wPar2;
	}
	break;

	case VMF_RETURN & 0xFF:
	case VMF_RETFB & 0xFF: /* RETFB - return from a function block */
		wProgramCounter = pull_CallingStack();
		wDataOfs = pull_DataOfsStack(); //-- by�o wDataOffsetReg
		break;

	case VMF_MCD & 0xFF: /* MCD - move from code -> data */
	{
		void *a, *b;
		WM_ADDRESS wPar1 = GetProgramAddress(); /* DST */
		WM_BYTE bReg1 = GetProgramByte();		/* count */
#ifdef FILE_DEBUG_INFO
		sprintf(blkData, "MCD wPar1:0x%X bReg1:0x%X\n", wPar1, bReg1);
		DebugFileCall(blkData, 1);
#endif
		a = getDataPtr(wPar1);
		b = getCodePtr(wProgramCounter);
#ifdef FILE_DEBUG_INFO
		sprintf(blkData, "Moving pointers value dst:0x%X src:0x%X\n", a, b);
		DebugFileCall(blkData, 1);
#endif
		memcpy(a, b, bReg1);
		wProgramCounter += bReg1;
	}
	break;

	case VMF_MCD2A & 0xFF: /* MCD2A - move from code -> data (FPGA, with 2-byte align ) */
	{
		WM_ADDRESS wPar1 = GetProgramAddress(); /* DST */
		WM_ADDRESS wPar2 = GetProgramAddress(); /* count */
		memcpy(getDataPtr(wPar1), getCodePtr(wProgramCounter), wPar2);
		wProgramCounter += wPar2;
		if (wPar2 & 0x0001)
			wProgramCounter++;
	}
	break;

	case VMF_MCD4A & 0xFF: /* MCD4A - move from code -> data (with 4-byte align ) */
	{
		WM_ADDRESS wPar1 = GetProgramAddress(); /* DST */
		WM_ADDRESS wPar2 = GetProgramAddress(); /* count */
		memcpy(getDataPtr(wPar1), getCodePtr(wProgramCounter), wPar2);
		wProgramCounter += (wPar2 & ~0x03) + ((wPar2 & 0x03) != 0 ? 4 : 0);
		//wProgramCounter += ((wPar2+3) >> 2) << 2;
	}
	break;

	case VMF_TRML & 0xFF:					   /* TRML - terminate and load program counter */
		wProgramCounter = GetProgramAddress(); /* DST */
		cycle_start_adr = wProgramCounter;
		bResult = WM_TERMINATE;
		break;

	case VMF_CALP & 0xFF:
	{
		WM_ADDRESS par1 = GetProgramAddress();
		WM_BOOL par2 = getBOOL(par1 + GetProgramAddress());
		WM_ADDRESS par3 = GetProgramAddress();

		if (par2)
		{
			wProgramCounter = par3;
			wDataOfs = par1; //-- by�o wDataOffsetReg
		}
	}

	break;

	case VMF_MEMCP & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_WORD wPar3;
		void *a, *b;

		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		wPar3 = GetProgramWord();
#ifdef FILE_DEBUG_INFO
		sprintf(blkData, "MEMCP wPar1:0x%X wPar2:0x%X wPar3:0x%X\n", wPar1, wPar2, wPar3);
		DebugFileCall(blkData, 1);
#endif
		a = getDataPtr(wPar1);
		b = getDataPtr(wPar2);
#ifdef FILE_DEBUG_INFO
		sprintf(blkData, "moving pointers a:0x%X b:0x%X\n", a, b);
		DebugFileCall(blkData, 1);
#endif
		memcpy(a, b, wPar3);
	}
	break;

	case VMF_MEMCP4A & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_DWORD wPar3;
		void *a, *b;

		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		wPar3 = GetProgramDWord();
#ifdef FILE_DEBUG_INFO
		sprintf(blkData, "MEMCP wPar1:0x%X wPar2:0x%X wPar3:0x%X\n", wPar1, wPar2, wPar3);
		DebugFileCall(blkData, 1);
#endif
		a = getDataPtr(wPar1);
		b = getDataPtr(wPar2);
#ifdef FILE_DEBUG_INFO
		sprintf(blkData, "moving pointers a:0x%X b:0x%X\n", a, b);
		DebugFileCall(blkData, 1);
#endif
		memcpy(a, b, wPar3);
	}
	break;

	case VMF_FPAT & 0xFF:
	{
		WM_ADDRESS wPar1;
		WM_BYTE bPar2, bPar3;

		wPar1 = GetProgramAddress();
		bPar2 = GetProgramByte();
		bPar3 = GetProgramByte();

		memset(getDataPtr(wPar1), bPar2, bPar3);
	}
	break;

	case VMF_FPAT4A & 0xFF:
	{
		WM_ADDRESS wPar1;
		WM_BYTE bPar2, bPar3;

		wPar1 = GetProgramAddress();
		bPar2 = GetProgramWord() & 0xFF;
		bPar3 = GetProgramWord() & 0xFF;

		memset(getDataPtr(wPar1), bPar2, bPar3);
	}
	break;

	case VMF_AURD & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_INT iPar3;
		WM_BYTE bPar4;

		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		iPar3 = getINT(GetProgramAddress());
		bPar4 = getBYTE(GetProgramAddress()); // was: GetProgramByte();, fixed by #JS @ 2009.07-07 20:32:36 CEST

		memcpy(getDataPtr(wPar1), getDataPtr(wPar2 + iPar3 * bPar4), bPar4);
	}
	break;

	case VMF_AUWR & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_INT iPar3;
		WM_BYTE bPar4;

		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		iPar3 = getINT(GetProgramAddress());
		bPar4 = getBYTE(GetProgramAddress()); // was: GetProgramByte();, fixed by #JS @ 2009.07-07 20:33:46 CEST

		memcpy(getDataPtr(wPar2 + iPar3 * bPar4), getDataPtr(wPar1), bPar4);
	}
	break;

	// added #JS @ 2009.10-16 20:52:21 CEST
	case VMF_GARD & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_INT iPar3;
		WM_BYTE bPar4;

		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		iPar3 = getINT(GetProgramAddress());
		bPar4 = getBYTE(GetProgramAddress());

		memcpy(getDataPtr(wPar1), pgmData + (wPar2 + iPar3 * bPar4), bPar4);
	}
	break;

	// added by #JS @ 2009.10-16 20:52:54 CEST
	case VMF_GAWR & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_INT iPar3;
		WM_BYTE bPar4;

		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		iPar3 = getINT(GetProgramAddress());
		bPar4 = getBYTE(GetProgramAddress());

		memcpy(pgmData + (wPar2 + iPar3 * bPar4), getDataPtr(wPar1), bPar4);
	}
	break;

	case VMF_AORD & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_INT iPar3;

		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		iPar3 = getINT(GetProgramAddress());

		memcpy(getDataPtr(wPar1), getDataPtr(wPar2 + iPar3), 1);
	}
	break;

	case VMF_AOWR & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_INT iPar3;

		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		iPar3 = getINT(GetProgramAddress());

		//memcpy(getDataPtr(wPar2),getDataPtr(wPar1+iPar3), 1);
		memcpy(getDataPtr(wPar2 + iPar3), getDataPtr(wPar1), 1);
	}
	break;

	case VMF_ATRD & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_INT iPar3;

		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		iPar3 = getINT(GetProgramAddress());

		memcpy(getDataPtr(wPar1), getDataPtr(wPar2 + iPar3 * 2), 2);
	}
	break;

	case VMF_ATWR & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_INT iPar3;

		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		iPar3 = getINT(GetProgramAddress());

		memcpy(getDataPtr(wPar2 + iPar3 * 2), getDataPtr(wPar1), 2);
	}
	break;

	case VMF_AFRD & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_INT iPar3;

		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		iPar3 = getINT(GetProgramAddress());

		memcpy(getDataPtr(wPar1), getDataPtr(wPar2 + iPar3 * 4), 4);
	}
	break;

	case VMF_AFWR & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_INT iPar3;

		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		iPar3 = getINT(GetProgramAddress());

		memcpy(getDataPtr(wPar2 + iPar3 * 4), getDataPtr(wPar1), 4);
	}
	break;

	case VMF_AQRD & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_INT iPar3;

		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		iPar3 = getINT(GetProgramAddress());

		memcpy(getDataPtr(wPar1), getDataPtr(wPar2 + iPar3 * 8), 8);
	}
	break;

	case VMF_AQWR & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_INT iPar3;

		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		iPar3 = getINT(GetProgramAddress());

		memcpy(getDataPtr(wPar2 + iPar3 * 8), getDataPtr(wPar1), 8);
	}
	break;

	case VMF_CEAC & 0xFF:
	{
		WM_INT iPar1, iPar2, iPar3, iRes;
		WM_ADDRESS wPar0 = GetProgramAddress();
		iPar1 = getINT(wPar0);
		iPar2 = getINT(GetProgramAddress());
		iPar3 = getINT(GetProgramAddress());

#ifdef FILE_DEBUG_INFO
		sprintf(blkData, "Check array access. Decoded values: P1=%d, P2=%d, P3=%d \n", iPar1, iPar2, iPar3);
		DebugFileCall(blkData, 1);
#endif

		iRes = iPar1 - iPar2;
		setINT(wPar0, iRes);
		if ((iRes < 0) || (iRes > iPar3))
		{
			wStatus1 |= WMSTAT_ARRAYBOUND;
			VMP_Debug(DBG_VME_ARRAY_INDEX_BOUND, iPar1);
		}
	}
	break;

	case VMF_CUR_TIME & 0xFF:
	{
		WM_ADDRESS wPar1;
		wPar1 = GetProgramAddress();

		setTIME(wPar1, VMP_CurrentTime());
	}
	break;

	case VMF_READ_RTC & 0xFF:
	{
		WM_ADDRESS wPar1;
		WM_DATE_AND_TIME dt;
		wPar1 = GetProgramAddress();
		VMP_ReadRTC(&dt);
		setDATE_AND_TIME(wPar1, dt);
	}
	break;

	case VMF_WRITE_RTC & 0xFF:
	{
		WM_ADDRESS wPar1, wPar2;
		WM_DATE_AND_TIME dt;
		wPar1 = GetProgramAddress();
		wPar2 = GetProgramAddress();
		getDATE_AND_TIME(wPar2, dt);
		setBOOL(wPar1, VMP_WriteRTC(&dt));
	}
	break;

	case VMF_RANDOML & 0xFF:
	{
		WM_ADDRESS wPar1;

		wPar1 = GetProgramAddress();

		setREAL(wPar1, VMP_GetRandom());
	}
	break;
	case VMF_GET_STATUS_WORD1 & 0xFF:
	{
		WM_ADDRESS wPar1;
		wPar1 = GetProgramAddress();

		setWORD(wPar1, wStatus1);
	}
	break;
	case VMF_GET_TST_DATETIME & 0xFF:
	{
		WM_ADDRESS wPar1;

		wPar1 = GetProgramAddress();

		setDATE_AND_TIME(wPar1, wm_rtc_value);
	}
	break;
	case VMF_TASK_CYCLE & 0xFF:
	{
		WM_ADDRESS wPar1;

		wPar1 = GetProgramAddress();

		setTIME(wPar1, task_cycle);
	}
	break;
	case VMF_GET_VMACH_VERSION & 0xFF:
	{
		WM_ADDRESS wPar1;
		wPar1 = GetProgramAddress();

		setWORD(wPar1, VMACH_VERSION);
	}
	break;
	case VMF_TASKSETUP & 0xFF:
	{
		WM_WORD wPar1;
		WM_ADDRESS wPar2;

		wPar1 = GetProgramWord();
		wPar2 = GetProgramAddress();

		task_cycle = wPar1;
	}
	break;

	default:
		bResult = WM_UNKNOWN;
		break;
	}
	return;
}
