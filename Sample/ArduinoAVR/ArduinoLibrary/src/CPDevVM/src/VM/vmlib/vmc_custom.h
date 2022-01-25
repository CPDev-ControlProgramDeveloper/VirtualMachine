/* Custom VM functions */

#ifdef PRAXIS_VM
#include <math.h>
#define FINT_MAX 0x7FFFFFFFL
#define FINT_MIN (-FINT_MAX - 1L)
//#define FINT_DETECT_OVERRUNS

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

#endif

WM_BYTE VMCLASS_PREFIX WMC_Custom(WM_BYTE opcode)
{

	WM_BYTE result = WM_UNKNOWN;

	switch(opcode)
	{
		case 0x00: //Low byte of $VMSYS.HWFBC code
		  {
			  //Begin constant prologue
			  WM_WORD fb_selector = GetProgramWord();
			  WM_ADDRESS fb_instance = GetProgramAddress();	// address of the parameter area
			  
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
				WM_ADDRESS fb_instance = GetProgramAddress();	// address of the parameter area
				
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
		case 0x80:		// SCNTF - powiadomienie MW o niespe³nieniu asercji
			{
				VMP_Assert(opcode, 0);
				result = 0;
			}
			break;
		case 0x81:		// SCRST - rozpoczêcie przetwarzania cyklu od pocz¹tku
		case 0x82:		// SCHLT - przerwanie wykonywania programu (przejœcie do obs³ugi zapisu wyjœæ I/O sterownika)
			break;
#endif
		default:
			bResult = WM_UNKNOWN;
			break;

	}
	return result;
}
