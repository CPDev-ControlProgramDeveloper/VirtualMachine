#include "vm.h"
#include "vmspec/vmdef.h"

#include "vmplatform.h"

#include <string.h>

/* command results */
#define WM_TERMINATE	1
#define WM_UNKNOWN		2

#ifndef  VMTASK_CLASS 

#include "vmregisters.h"

#endif // ! VMTASK_CLASS 


#include "vmfunc\vm_data_access.h"
#include "vmlib\vm_lib.h"
#include "vmfunc\vm_stack.h"

unsigned long VMCLASS_PREFIX WM_GetCycles(void) { return nCycles; };
WM_WORD VMCLASS_PREFIX WM_GetStatus1(void) { return wStatus1; };

void VMCLASS_PREFIX WM_InitRegisters(void)
{
	wCallingStackPtr = 0;
	wDataOfsStackPtr = 0;
	wStatus1 = 0;
	wProgramCounter = 0;
	wDataOfs = 0;
	nCycles = 0;
	VMP_ReadRTC(&wm_rtc_value);
	bResult = 0;
	return;
}
/* perform a VM primitive command */

int VMCLASS_PREFIX WM_RunCommand(void)
{

	VMP_PreNextCommand();

#ifndef ALIGN_4B	
	WM_WORD wCmdCode = GetProgramWord();
#else
	WM_WORD wCmdCode = GetProgramWord();
	wProgramCounter += 2;
	// WM_WORD wCmdCode = GetProgramDWord();
#endif
	WM_BYTE wCmd = wCmdCode & 0x00FF;
	WM_BYTE wCmdOpt = (wCmdCode & 0xFF00) >> 8;

	bResult = 0;

	//--wDataOfs = wDataOffsetReg;	/* set data offset if the highest bit set */
	//--wCmd &= 0x7F;					/* and clear the bit */

	switch (wCmd)
	{
//		case FG_HALT:
//			bResult = WM_UNKNOWN;

		case 0:									/* NOP */
			break;

		case FG_SYSCTRL:					/* extra functions group 1 */
			WMC_SYSCTRL(wCmdOpt);
			break;

		case FG_SYSMEM:
			WMC_MMV(wCmdOpt);
			break;

		case FG_ADD:
			WMC_ADD(wCmdOpt);
			break;

		case FG_AND:
			WMC_AND(wCmdOpt);
			break;

		case FG_OR:
			WMC_OR(wCmdOpt);
			break;

		case FG_XOR:
			WMC_XOR(wCmdOpt);
			break;

		case FG_MUL:
			WMC_MUL(wCmdOpt);
			break;

		case FG_SUB:
			WMC_SUB(wCmdOpt);
			break;

		case FG_DIV_MOD:
			WMC_DIV_MOD(wCmdOpt);
			break;

		case FG_NEG_NOT:
			WMC_NEG_NOT(wCmdOpt);
			break;

#ifdef VM_MATH_SUPPORT
		case FG_EXPT_ABS_DOW:
			if (wCmdOpt < (VMF_GET_DAYOFWEEK_DATE & 0xFF))
				WMC_EXPT_ABS(wCmdOpt);
			else
				WMC_DOW(wCmdOpt);
			break;
#endif
		case FG_SHI_ROT:
			WMC_SHI_ROT(wCmdOpt);
			break;

		case FG_SEL:
			WMC_SEL(wCmdOpt);
			break;

		case FG_LIMIT:
			WMC_LIMIT(wCmdOpt);
			break;

		case FG_MAX:
			WMC_MAX(wCmdOpt);
			break;

		case FG_MIN:
			WMC_MIN(wCmdOpt);
			break;

		case FG_GT:
			WMC_GT(wCmdOpt);
			break;

		case FG_GE:
			WMC_GE(wCmdOpt);
			break;

		case FG_LT:
			WMC_LT(wCmdOpt);
			break;

		case FG_LE:
			WMC_LE(wCmdOpt);
			break;

		case FG_EQ:
			WMC_EQ(wCmdOpt);
			break;

		case FG_NE:
			WMC_NE(wCmdOpt);
			break;

		case FG_MUX:
			WMC_MUX(wCmdOpt);
			break;

		case FG_CONV:
			WMC_CONV(wCmdOpt);
			break;
			
		case FG_CUSTOMSPACE:
		    bResult = WMC_Custom(wCmdOpt);
		    break;

#ifdef VM_POINTER_SUPPORT
		case FG_POINTERS:
		    WMC_Pointers(wCmdOpt);
		    break;
#endif

#ifdef VM_STRING_SUPPORT
		case FG_STRING:
		    WMC_String(wCmdOpt);
		    break;
#endif

		default:
			bResult = WM_UNKNOWN;
			break;
	}
	if (bResult == WM_UNKNOWN)
	{
		switch(wCmd)
		{
			case FG_CUSTOMSPACE:
				VMP_Debug(DBG_VME_NATIVE_BLOCK, ((WM_WORD)wCmd << 8) | wCmdOpt);
				break;
			default:
				VMP_Debug(DBG_VME_UNKNOWN_FUNCTION, ((WM_WORD)wCmd << 8) | wCmdOpt);
				break;
		}
		return -1;
	}
	return bResult;
}


/* Initialize VM and program */
void VMCLASS_PREFIX WM_Initialize(int mode)
{
	bRunMode = mode;

	if (!(bRunMode & WM_MODE_CONTINUE))
	{
		WM_InitRegisters();
	}

	if (bRunMode & WM_MODE_COLDRESTART)
	{
		wStatus1 = WMSTAT_COLDRESTART;
	}

	if (bRunMode & WM_MODE_FIRST_START)
	{
		wStatus1 |= WMSTAT_FIRSTSTART;
	}

	wStatus1 &= ~WMSTAT_STOPPED;	// now running
	bRunMode |= WM_MODE_RUNNING;

	clear_Stacks();

	VMP_PreProgram();

}

/* Execute single cycle */

void VMCLASS_PREFIX WM_RunCycle()
{
	VMP_ReadRTC(&wm_rtc_value);

	VMP_PreCycle();

	if (!bRunMode) return;

	while(!WM_RunCommand() && bRunMode);

	VMP_PostCycle();
	
	wStatus1 &= ~(WMSTAT_COLDRESTART | WMSTAT_FIRSTSTART);

	nCycles++;
}

/* Finish execution */
void VMCLASS_PREFIX WM_Shutdown()
{	
	wStatus1 |= WMSTAT_STOPPED;		// now stopped
	bRunMode &= ~WM_MODE_RUNNING;
	VMP_PostProgram();
}

/* program loop */

int VMCLASS_PREFIX WM_Run(int mode)
{
	WM_Initialize(mode);

	while(bRunMode)
	{
		WM_RunCycle();
	}

	WM_Shutdown();

	return 0;
}

int VMCLASS_PREFIX WM_Stop(void)
{
	return (bRunMode = 0);
}

int VMCLASS_PREFIX WM_GetData(WM_ADDRESS address, WM_BYTE len, WM_BYTE* buf)
{
	memcpy(buf, pgmData+address, len);

	return len;
}

int VMCLASS_PREFIX WM_SetData(WM_ADDRESS address, WM_BYTE len, WM_BYTE* buf)
{
	memcpy(pgmData+address, buf, len);

	return len;
}

