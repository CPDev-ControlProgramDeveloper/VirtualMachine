/* MAX implementation */

void VMCLASS_PREFIX WMC_MAX(WM_BYTE opt)
{
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */

	switch(opt & 0x0F)
	{
		case VMF_MAX_BOOL_BOOL & 0x000F:		/* BOOL ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_BOOL par2 = getBOOL(GetProgramAddress());
				WM_BOOL wRes = par1 < par2 ? par2 : par1;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_MAX_SINT_SINT & 0x000F:		/* SINT ops */
			{
				WM_SINT par1 = getSINT(GetProgramAddress());
				WM_SINT par2 = getSINT(GetProgramAddress());
				WM_SINT wRes = par1 < par2 ? par2 : par1;

				setSINT(wDst, wRes);
			}
			break;

		case VMF_MAX_INT_INT & 0x000F:		/* INT ops */
			{
				WM_INT par1 = getINT(GetProgramAddress());
				WM_INT par2 = getINT(GetProgramAddress());
				WM_INT wRes = par1 < par2 ? par2 : par1;

				setINT(wDst, wRes);
			}
			break;

		case VMF_MAX_DINT_DINT & 0x000F:		/* DINT ops */
			{
				WM_DINT par1 = getDINT(GetProgramAddress());
				WM_DINT par2 = getDINT(GetProgramAddress());
				WM_DINT wRes = par1 < par2 ? par2 : par1;

				setDINT(wDst, wRes);
			}
			break;

		case VMF_MAX_LINT_LINT & 0x000F:		/* LINT ops */
			{
				WM_LINT par1 = getLINT(GetProgramAddress());
				WM_LINT par2 = getLINT(GetProgramAddress());
				WM_LINT wRes = par1 < par2 ? par2 : par1;

				setLINT(wDst, wRes);
			}
			break;

		case VMF_MAX_BYTE_BYTE & 0x000F:		/* INT ops */
			{
				WM_BYTE par1 = getBYTE(GetProgramAddress());
				WM_BYTE par2 = getBYTE(GetProgramAddress());
				WM_BYTE wRes = par1 < par2 ? par2 : par1;

				setBYTE(wDst, wRes);
			}
			break;

		case VMF_MAX_WORD_WORD & 0x000F:		/* INT ops */
			{
				WM_WORD par1 = getWORD(GetProgramAddress());
				WM_WORD par2 = getWORD(GetProgramAddress());
				WM_WORD wRes = par1 < par2 ? par2 : par1;

				setWORD(wDst, wRes);
			}
			break;

		case VMF_MAX_DWORD_DWORD & 0x000F:		/* INT ops */
			{
				WM_DWORD par1 = getDWORD(GetProgramAddress());
				WM_DWORD par2 = getDWORD(GetProgramAddress());
				WM_DWORD wRes = par1 < par2 ? par2 : par1;

				setDWORD(wDst, wRes);
			}
			break;

		case VMF_MAX_LWORD_LWORD & 0x000F:		/* INT ops */
			{
				WM_LWORD par1 = getLWORD(GetProgramAddress());
				WM_LWORD par2 = getLWORD(GetProgramAddress());
				WM_LWORD wRes = par1 < par2 ? par2 : par1;

				setLWORD(wDst, wRes);
			}
			break;

		case VMF_MAX_REAL_REAL & 0x000F:		/* INT ops */
			{
				WM_REAL par1 = getREAL(GetProgramAddress());
				WM_REAL par2 = getREAL(GetProgramAddress());
				WM_REAL wRes = par1 < par2 ? par2 : par1;

				setREAL(wDst, wRes);
			}
			break;

#ifdef VM_LREAL_SUPPORT
		case VMF_MAX_LREAL_LREAL & 0x000F:		/* LREAL ops */
			{
				WM_LREAL par1 = getLREAL(GetProgramAddress());
				WM_LREAL par2 = getLREAL(GetProgramAddress());
				WM_LREAL wRes = par1 < par2 ? par2 : par1;

				setLREAL(wDst, wRes);
			}
			break;
#endif
		case VMF_MAX_TIME_TIME & 0x000F:		/* INT ops */
			{
				WM_TIME par1 = getTIME(GetProgramAddress());
				WM_TIME par2 = getTIME(GetProgramAddress());
				WM_TIME wRes = par1 < par2 ? par2 : par1;

				setTIME(wDst, wRes);
			}
			break;

		case VMF_MAX_DATE_DATE & 0x000F:		/* INT ops */
			{
				WM_DATE par1;
				WM_DATE par2;
            WM_DATE wRes;
				getDATE(GetProgramAddress(),par1);
				getDATE(GetProgramAddress(),par2);
				wRes = lessDATE(par1,par2) ? par2 : par1;

				setDATE(wDst, wRes);
			}
			break;

		case VMF_MAX_TIME_OF_DAY_TIME_OF_DAY & 0x000F:		/* INT ops */
			{
				WM_TOD par1;
				WM_TOD par2;
            WM_TOD wRes;

				getTOD(GetProgramAddress(),par1);
				getTOD(GetProgramAddress(),par2);
				wRes = lessTOD(par1,par2) ? par2 : par1;

				setTOD(wDst, wRes);
			}
			break;

		case VMF_MAX_DATE_AND_TIME_DATE_AND_TIME & 0x000F:		/* INT ops */
			{
				WM_DATE_AND_TIME par1;
				WM_DATE_AND_TIME par2;
            WM_DATE_AND_TIME wRes;

				getDATE_AND_TIME(GetProgramAddress(),par1);
				getDATE_AND_TIME(GetProgramAddress(),par2);
				wRes = lessDATE_AND_TIME(par1,par2) ? par2 : par1;

				setDATE_AND_TIME(wDst, wRes);
			}
			break;

#ifdef VM_STRING_SUPPORT
		case VMF_MAX_STRING_STRING & 0x000F:		/* INT ops */
			{
				WM_STRING par1;
				WM_STRING par2;
				getSTRING(GetProgramAddress(), &par1);
				getSTRING(GetProgramAddress(), &par2);
				WM_STRING* wRes = lessSTRING(&par1,&par2) ? &par2 : &par1;

				setSTRING(wDst, wRes);
			}
			break;
#endif
		default:
			bResult = WM_UNKNOWN;
	}
	return;
}
