/* Conversions implementation */

void VMCLASS_PREFIX WMC_CONV(WM_BYTE opt)
{
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */

	switch(opt)
	{
		case VMF_INT_TO_REAL & 0x00FF:
			{
				WM_REAL wRes = 0;

				wRes = (WM_REAL)(getINT(GetProgramAddress()));

				setREAL(wDst, wRes);
			}
			break;

		case VMF_DINT_TO_REAL & 0x00FF:
			{
				WM_REAL wRes = 0;

				wRes = (WM_REAL)(getDINT(GetProgramAddress()));

				setREAL(wDst, wRes);
			}
			break;

		case VMF_TIME_TO_DINT & 0x00FF:
			{
				WM_DINT wRes = 0;

				wRes = getTIME(GetProgramAddress());

				setDINT(wDst, wRes);
			}
			break;

		case VMF_DINT_TO_TIME & 0x00FF:
			{
				WM_DINT wRes = 0;

				wRes = getTIME(GetProgramAddress());

				setTIME(wDst, wRes);
			}
			break;

		case VMF_TIME_TO_REAL & 0x00FF:
			{
				WM_REAL wRes = 0;

				wRes = (WM_REAL)(getTIME(GetProgramAddress()));

				setREAL(wDst, wRes);
			}
			break;

		case VMF_REAL_TO_TIME & 0x00FF:
			{
				WM_TIME wRes = 0;

				wRes = (WM_TIME)(getREAL(GetProgramAddress()));

				setTIME(wDst, wRes);
			}
			break;	

		case VMF_REAL_TO_INT & 0x00FF:
			{
				WM_INT wRes = 0;

				wRes = (WM_INT)(getREAL(GetProgramAddress()));

				setINT(wDst, wRes);
			}
			break;	

		case VMF_INT_TO_BOOL & 0x00FF:
			{
				WM_BOOL wRes = 0;

				wRes = ((WM_BOOL)(getINT(GetProgramAddress())) > 0);

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_INT_TO_DINT & 0x00FF:
			{
				WM_DINT wRes = 0;

				wRes = getINT(GetProgramAddress());

				setDINT(wDst, wRes);
			}
			break;

		case VMF_BCD_TO_INT_BYTE & 0x00FF:
			{
				WM_BYTE wRes = 0;

				wRes = getBYTE(GetProgramAddress());
				wRes = (wRes >> 4)*10 + (wRes & 0xF);

				setINT(wDst, wRes);
			}
			break;

		case VMF_BCD_TO_INT_WORD & 0x00FF:
			{
				WM_WORD wRes = 0;

				wRes = getWORD(GetProgramAddress());

				wRes = (wRes >> 12)*1000 + ((wRes >> 8) & 0xF)*100 + ((wRes >> 4) & 0xF)*10 + (wRes & 0xF);

				setINT(wDst, wRes);
			}
			break;

		case VMF_INT_TO_BYTE_BCD & 0x00FF:
			{
				WM_BYTE wRes = 0;

				wRes = (WM_BYTE)getINT(GetProgramAddress());
				wRes = ((wRes / 10) << 4) + (wRes % 10);

				setBYTE(wDst, wRes);
			}
			break;

		case VMF_INT_TO_WORD_BCD & 0x00FF:
			{
				WM_WORD wRes = 0;

				wRes = (WM_WORD)getINT(GetProgramAddress());

				wRes = ((wRes / 1000) << 12) + (((wRes / 100) % 10) << 8) + (((wRes / 10) % 10) << 4) + (wRes % 10);

 				setWORD(wDst, wRes);
			}
			break;

#ifdef VM_LREAL_SUPPORT
		case VMF_REAL_TO_LREAL & 0x00FF:
			{
				WM_LREAL wRes = 0;

				wRes = (WM_LREAL)(getREAL(GetProgramAddress()));

				setLREAL(wDst, wRes);
			}
			break;

		case VMF_LREAL_TO_REAL & 0x00FF:
			{
				WM_REAL wRes = 0;

				wRes = (WM_REAL)(getLREAL(GetProgramAddress()));

				setREAL(wDst, wRes);
			}
			break;

		case VMF_LINT_TO_LREAL & 0x00FF:
			{
				WM_LREAL wRes = 0;

				wRes = (WM_LREAL)(getLINT(GetProgramAddress()));

				setLREAL(wDst, wRes);
			}
			break;
#endif
		case VMF_DWORD_TO_REAL & 0x00FF:
			{
				WM_REAL wRes = 0;

				wRes = (WM_REAL)(getDWORD(GetProgramAddress()));

				setREAL(wDst, wRes);
			}
			break;

		case VMF_REAL_TO_DWORD & 0x00FF:
			{
				WM_DWORD wRes = 0;

				wRes = (WM_DWORD)(getREAL(GetProgramAddress()));

				setDWORD(wDst, wRes);
			}
			break;

		default:
			bResult = WM_UNKNOWN;
	}
	return;
}

