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


#include "vmfunc/vm_data_access.h"
#include "vmlib/vm_lib.h"
#include "vmfunc/vm_stack.h"

#include "vmlib/vmc_custom.h"
#include <time.h>

unsigned long VMCLASS_PREFIX WM_GetCycles(void) { return nCycles; }
WM_WORD VMCLASS_PREFIX WM_GetStatus1(void) { return wStatus1; }

void VMCLASS_PREFIX WM_InitRegisters(void)
{
	wCallingStackPtr = 0;
	wDataOfsStackPtr = 0;
	wStatus1 = 0;
	wProgramCounter = 0;
	wDataOfs = 0;
	nCycles = 0;
#ifdef VM_DATETIME_SUPPORT	
	VMP_ReadRTC(&wm_rtc_value);
#endif	
	bResult = 0;
	return;
}
/* perform a VM primitive command */

int VMCLASS_PREFIX WM_RunCommand(void)
{
	WM_WORD wCmdCode;
	WM_BYTE wCmd, wCmdOpt;

	VMP_PreNextCommand();

#ifndef ALIGN_4B	
	wCmdCode = GetProgramWord();
#else
	wCmdCode = GetProgramWord();
	wProgramCounter += 2;
	//WM_WORD wCmdCode = GetProgramDWord();
#endif
	wCmd = wCmdCode & 0x00FF;
	wCmdOpt = (wCmdCode & 0xFF00) >> 8;

	bResult = 0;

	//--wDataOfs = wDataOffsetReg;	/* set data offset if the highest bit set */
	//--wCmd &= 0x7F;					/* and clear the bit */
/*
    FILE* f = fopen("Users/nackin/Downloads/VirtualMachine-main-2/Platforms/BeagleboneAI/xcp/debug.log", "at");
    if(f != NULL)
    {
        fprintf(f, "time: 0x%X, pc: 0x%X [%02X, %02X, %02X, %02X, %02x]\n", time(NULL), wProgramCounter, pgmData[0], pgmData[1], pgmData[2], pgmData[3], pgmData[4]);
        fclose(f);
    }
    */
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
	
#ifdef VM_DATETIME_SUPPORT		
	VMP_ReadRTC(&wm_rtc_value);
#endif
    
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

#ifdef CHECK_WM_TYPES
void WM_CheckTypes()
{

#define WM_CHECK_SIZE(type, size) ((void)sizeof(char[1 - 2*!!(sizeof(type) - size)]))

WM_CHECK_SIZE(WM_BOOL, 1);
WM_CHECK_SIZE(WM_BYTE, 1);
WM_CHECK_SIZE(WM_INT, 2);
WM_CHECK_SIZE(WM_DINT, 4);
WM_CHECK_SIZE(WM_WORD, 2);
WM_CHECK_SIZE(WM_DWORD, 4);

#ifdef VM_LONG_SUPPORT
WM_CHECK_SIZE(WM_LWORD, 8);
WM_CHECK_SIZE(WM_LINT, 8);
#endif

#ifdef VM_REAL_SUPPORT
WM_CHECK_SIZE(WM_REAL, 4);
#endif

#ifdef VM_LREAL_SUPPORT
WM_CHECK_SIZE(WM_LREAL, 8);
#endif

#ifdef WM_DATETIME_SUPPORT
WM_CHECK_SIZE(WM_DATE, 8);
WM_CHECK_SIZE(WM_TIME_OF_DAY, 8);
WM_CHECK_SIZE(WM_DATE_AND_TIME, 8);
#endif

}
#endif

WM_DINT FINT_Limit(WM_LINT arg1)
{
    if (arg1 > FINT_MAX)
    {
        return FINT_MAX;
    }
    else if (arg1 < FINT_MIN)
    {
        return FINT_MIN;
    }
    else return (WM_DINT)arg1;
}

WM_BYTE VMCLASS_PREFIX WMC_Custom(WM_BYTE opcode)
{

    WM_BYTE result = WM_UNKNOWN;

    switch(opcode)
    {
        case 0x00: //Low byte of $VMSYS.HWFBC code
          {
              //Begin constant prologue
              WM_WORD fb_selector = GetProgramWord();
              WM_ADDRESS fb_instance = GetProgramAddress();    // address of the parameter area
              
              push_DataOfsStack(wDataOfs);
              wDataOfs += fb_instance;
              //End constant prologue

              result = VMP_ExecNativeBlock(fb_selector, fb_instance);

              //Begin constant epilogue
              wDataOfs = pull_DataOfsStack();
              //End constant epilogue
          }
        break;
        case 0x01:
            {
                //Begin constant prologue
                WM_WORD fb_selector = GetProgramWord();
                WM_ADDRESS fb_instance = GetProgramAddress();    // address of the parameter area
                
                push_DataOfsStack(wDataOfs);
                wDataOfs += fb_instance;
                //End constant prologue

                result = VMP_InitNativeBlock(fb_selector, fb_instance);
                
                //Begin constant epilogue
                wDataOfs = pull_DataOfsStack();
                //End constant epilogue
            }
            break;
        case 0x02:
            {
            WM_ADDRESS resultAdd = GetProgramAddress();
            WM_WORD cmd = getWORD(GetProgramAddress());
            setWORD(resultAdd, VMP_FlashDataControl(cmd));
            result=0;
            }
            break;
        case 0x03:
            {
            WM_ADDRESS r_status = GetProgramAddress();
            WM_ADDRESS a_varstore = GetProgramAddress() + wDataOfs;
            WM_WORD a_size = GetProgramWord();
            setBOOL(r_status, VMP_FlashDataWrite(a_varstore,a_size));
            result=0;
            }
            break;
        case 0x04:
            {
            WM_ADDRESS r_status = GetProgramAddress();
            WM_ADDRESS a_varstore = GetProgramAddress() + wDataOfs;
            WM_WORD a_size = GetProgramWord();
            setBOOL(r_status, VMP_FlashDataRead(a_varstore,a_size));
            result=0;
            }
            break;
#ifdef PRAXIS_VM
        case 0x10: //ADD
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
#ifdef FINT_DETECT_OVERRUNS
                WM_LINT arg1 = (WM_LINT)getDINT(GetProgramAddress());
#else
                WM_DINT arg1 = getDINT(GetProgramAddress());
#endif
                arg1 += getDINT(GetProgramWord());
#ifdef FINT_DETECT_OVERRUNS
                setDINT(r_wynik, FINT_Limit(arg1));
#else
                setDINT(r_wynik, arg1);
#endif
                result = 0;
            }
            break;
        case 0x11: //SUB
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
#ifdef FINT_DETECT_OVERRUNS
                WM_LINT arg1 = (WM_LINT)getDINT(GetProgramAddress());
#else
                WM_DINT arg1 = getDINT(GetProgramAddress());
#endif
                arg1 -= getDINT(GetProgramAddress());
#ifdef FINT_DETECT_OVERRUNS
                setDINT(r_wynik, FINT_Limit(arg1));
#else
                setDINT(r_wynik, arg1);
#endif
                result = 0;
            }
            break;
        case 0x12: //MUL
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
                WM_LINT arg1 = (WM_LINT)getDINT(GetProgramAddress());
                arg1 *= getDINT(GetProgramAddress());
                arg1 /= 1000;
#ifdef FINT_DETECT_OVERRUNS
                setDINT(r_wynik, FINT_Limit(arg1));
#else
                setDINT(r_wynik, (WM_DINT)arg1);
#endif
                result = 0;
            }
            break;
        case 0x13: //DIV
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
                WM_LINT arg1 = (WM_LINT)getDINT(GetProgramAddress());
                arg1 *= 1000;
                WM_DINT dzielnik = getDINT(GetProgramWord());
                if(dzielnik == 0)
                {
                    if(arg1 > 0)
                        arg1 = FINT_MAX;//0x7FFFFFFF;
                    else if(arg1 < 0)
                        arg1 = FINT_MIN;//0x80000000;
                }
                else
                    arg1 /= dzielnik;
#ifdef FINT_DETECT_OVERRUNS
                setDINT(r_wynik, FINT_Limit(arg1));
#else
                setDINT(r_wynik, (WM_DINT)arg1);
#endif
                result = 0;
            }
            break;
        case 0x14: //NEG
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
                WM_DINT arg1 = getDINT(GetProgramAddress());
#ifdef FINT_DETECT_OVERRUNS
                if (arg1 == FINT_MIN)
                    {
                    arg1 = FINT_MAX;
                    }
                else
                    {
                    arg1 = -arg1;
                    }
                setDINT(r_wynik, arg1);
#else
                setDINT(r_wynik, -arg1);
#endif
                result = 0;
            }
            break;
        case 0x15: //ABS
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
                WM_DINT arg1 = getDINT(GetProgramAddress());
                if(arg1 < 0)
#ifdef FINT_DETECT_OVERRUNS
                    if (arg1 == FINT_MIN)
                        {
                        arg1 = FINT_MAX;
                        }
                    else
#endif
                    arg1 = -arg1;
                setDINT(r_wynik, arg1);
                result = 0;
            }
            break;
        case 0x16: //GT
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
                WM_DINT arg1 = getDINT(GetProgramAddress());
                WM_DINT arg2 = getDINT(GetProgramAddress());
                setBOOL(r_wynik, arg1 > arg2);
                result = 0;
            }
            break;
        case 0x17: //GE
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
                WM_DINT arg1 = getDINT(GetProgramAddress());
                WM_DINT arg2 = getDINT(GetProgramAddress());
                setBOOL(r_wynik, arg1 >= arg2);
                result = 0;
            }
            break;
        case 0x18: //EQ
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
                WM_DINT arg1 = getDINT(GetProgramAddress());
                WM_DINT arg2 = getDINT(GetProgramAddress());
                setBOOL(r_wynik, arg1 == arg2);
                result = 0;
            }
            break;
        case 0x19: //LE
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
                WM_DINT arg1 = getDINT(GetProgramAddress());
                WM_DINT arg2 = getDINT(GetProgramAddress());
                setBOOL(r_wynik, arg1 <= arg2);
                result = 0;
            }
            break;
        case 0x1A: //LT
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
                WM_DINT arg1 = getDINT(GetProgramAddress());
                WM_DINT arg2 = getDINT(GetProgramAddress());
                setBOOL(r_wynik, arg1 < arg2);
                result = 0;
            }
            break;
        case 0x1B: //NE
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
                WM_DINT arg1 = getDINT(GetProgramAddress());
                WM_DINT arg2 = getDINT(GetProgramAddress());
                setBOOL(r_wynik, arg1 != arg2);
                result = 0;
            }
            break;
        case 0x1C: //FINT_TO_DINT
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
                WM_DINT arg1 = getDINT(GetProgramAddress());
                arg1 /= 1000;
                setDINT(r_wynik, arg1);
                result = 0;
            }
            break;
        case 0x1D: //DINT_TO_FINT
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
#ifdef FINT_DETECT_OVERRUNS
                WM_LINT arg1 = (WM_LINT)getDINT(GetProgramAddress());
#else
                WM_DINT arg1 = getDINT(GetProgramAddress());
#endif
                arg1 *= 1000;
#ifdef FINT_DETECT_OVERRUNS
                setDINT(r_wynik, FINT_Limit(arg1));
#else
                setDINT(r_wynik, arg1);
#endif
                result = 0;
            }
            break;
        case 0x1E: //REAL_TO_FINT
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
                WM_REAL arg1 = getREAL(GetProgramAddress());
                arg1 *= 1000.0;
#ifdef FINT_DETECT_OVERRUNS
                WM_LINT v_wynik = (WM_LINT)arg1;
#else
                WM_DINT v_wynik = (WM_DINT)arg1;
#endif
                WM_REAL fRemainder = fmodf(arg1, 1);
                if (v_wynik >= 0)
                {
                    if (fRemainder >= 0.5)
                        v_wynik++;
                }
                else
                {
                    if (fRemainder <= -0.5)
                        v_wynik--;
                }
#ifdef FINT_DETECT_OVERRUNS
                setDINT(r_wynik, FINT_Limit(v_wynik));
#else
                setDINT(r_wynik, v_wynik);
#endif
                result = 0;
            }
            break;
        case 0x1F: //FINT_TO_REAL
            {
                WM_ADDRESS r_wynik = GetProgramAddress();
                WM_REAL arg1 = (WM_REAL)getDINT(GetProgramAddress());
                arg1 /= 1000.0;
                setREAL(r_wynik, arg1);
                result = 0;
            }
            break;
        case 0x20: //SQRT_FINT_FINT
            {
                WM_REAL rr, fRemainder;
                WM_ADDRESS wDst = GetProgramAddress();
                WM_DINT arg1 = (WM_DINT)getDINT(GetProgramAddress());
                rr = (WM_REAL)arg1 / 1000.0f;
                rr = (WM_REAL)sqrt(rr);
                rr *= 1000.0;
                arg1 = (WM_DINT)rr;
                fRemainder = fmodf(rr, 1);
                if (arg1 >= 0)
                {
                    if (fRemainder >= 0.5)
                        arg1++;
                }
                else
                {
                    if (fRemainder <= -0.5)
                        arg1--;
                }
                setDINT(wDst, arg1);
                result = 0;
            }
            break;
        case 0x21: //SIN_FINT_FINT
            {
                WM_REAL rr, fRemainder;
                WM_ADDRESS wDst = GetProgramAddress();
                WM_DINT arg1 = (WM_DINT)getDINT(GetProgramAddress());
                rr = (WM_REAL)arg1 / 1000.0f;
                rr = (WM_REAL)sin(rr);
                rr *= 1000.0;
                arg1 = (WM_DINT)rr;
                fRemainder = fmodf(rr, 1);
                if (arg1 >= 0)
                {
                    if (fRemainder >= 0.5)
                        arg1++;
                }
                else
                {
                    if (fRemainder <= -0.5)
                        arg1--;
                }
                setDINT(wDst, arg1);
                result = 0;
            }
            break;
        case 0x22: //COS_FINT_FINT
            {
                WM_REAL rr, fRemainder;
                WM_ADDRESS wDst = GetProgramAddress();
                WM_DINT arg1 = (WM_DINT)getDINT(GetProgramAddress());
                rr = (WM_REAL)arg1 / 1000.0f;
                rr = (WM_REAL)cos(rr);
                rr *= 1000.0;
                arg1 = (WM_DINT)rr;
                fRemainder = fmodf(rr, 1);
                if (arg1 >= 0)
                {
                    if (fRemainder >= 0.5)
                        arg1++;
                }
                else
                {
                    if (fRemainder <= -0.5)
                        arg1--;
                }
                setDINT(wDst, arg1);
                result = 0;
            }
            break;
        case 0x23: //TAN_FINT_FINT
            {
                WM_REAL rr, fRemainder;
                WM_ADDRESS wDst = GetProgramAddress();
                WM_DINT arg1 = (WM_DINT)getDINT(GetProgramAddress());
                rr = (WM_REAL)arg1 / 1000.0f;
                rr = (WM_REAL)tan(rr);
                rr *= 1000.0;
#ifdef FINT_DETECT_OVERRUNS
                WM_LINT v_wynik = (WM_LINT)rr;
#else
                WM_DINT v_wynik = (WM_DINT)rr;
#endif
                fRemainder = fmodf(rr, 1);
                if (v_wynik >= 0)
                {
                    if (fRemainder >= 0.5)
                        v_wynik++;
                }
                else
                {
                    if (fRemainder <= -0.5)
                        v_wynik--;
                }
#ifdef FINT_DETECT_OVERRUNS
                setDINT(wDst, FINT_Limit(v_wynik));
#else
                setDINT(wDst, v_wynik);
#endif
                result = 0;
            }
            break;
#endif //#ifdef PRAXIS_VM

#ifdef ASSERT_ON
        case 0x80:        // SCNTF - assertion failed
            {
                VMP_Assert(opcode, 0);
                result = 0;
            }
            break;
        case 0x81:        // SCRST - restart the cycle
        case 0x82:        // SCHLT - halt the program
            break;
#endif
        default:
            bResult = WM_UNKNOWN;
            break;

    }
    return result;
}
