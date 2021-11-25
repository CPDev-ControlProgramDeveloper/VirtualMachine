/* SEL implementation */

void VMCLASS_PREFIX WMC_SEL(WM_BYTE opt)
{
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */

	switch(opt & 0x0F)
	{
		case VMF_SEL_BOOL_BOOL_BOOL & 0x000F:		/* BOOL ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_BOOL par2 = getBOOL(GetProgramAddress());
				WM_BOOL par3 = getBOOL(GetProgramAddress());
				WM_BOOL wRes = par1 ? par3 : par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_SEL_BOOL_SINT_SINT & 0x000F:		/* SINT ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_SINT par2 = getSINT(GetProgramAddress());
				WM_SINT par3 = getSINT(GetProgramAddress());
				WM_SINT wRes = par1 ? par3 : par2;

				setSINT(wDst, wRes);
			}
			break;

		case VMF_SEL_BOOL_INT_INT & 0x000F:		/* INT ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_INT par2 = getINT(GetProgramAddress());
				WM_INT par3 = getINT(GetProgramAddress());
				WM_INT wRes = par1 ? par3 : par2;

				setINT(wDst, wRes);
			}
			break;

		case VMF_SEL_BOOL_DINT_DINT & 0x000F:		/* DINT ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_DINT par2 = getDINT(GetProgramAddress());
				WM_DINT par3 = getDINT(GetProgramAddress());
				WM_DINT wRes = par1 ? par3 : par2;

				setDINT(wDst, wRes);
			}
			break;

		case VMF_SEL_BOOL_LINT_LINT & 0x000F:		/* LINT ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_LINT par2 = getLINT(GetProgramAddress());
				WM_LINT par3 = getLINT(GetProgramAddress());
				WM_LINT wRes = par1 ? par3 : par2;

				setLINT(wDst, wRes);
			}
			break;

		case VMF_SEL_BOOL_BYTE_BYTE & 0x000F:		/* INT ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_BYTE par2 = getBYTE(GetProgramAddress());
				WM_BYTE par3 = getBYTE(GetProgramAddress());
				WM_BYTE wRes = par1 ? par3 : par2;

				setBYTE(wDst, wRes);
			}
			break;

		case VMF_SEL_BOOL_WORD_WORD & 0x000F:		/* INT ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_WORD par2 = getWORD(GetProgramAddress());
				WM_WORD par3 = getWORD(GetProgramAddress());
				WM_WORD wRes = par1 ? par3 : par2;

				setWORD(wDst, wRes);
			}
			break;

		case VMF_SEL_BOOL_DWORD_DWORD & 0x000F:		/* INT ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_DWORD par2 = getDWORD(GetProgramAddress());
				WM_DWORD par3 = getDWORD(GetProgramAddress());
				WM_DWORD wRes = par1 ? par3 : par2;

				setDWORD(wDst, wRes);
			}
			break;

		case VMF_SEL_BOOL_LWORD_LWORD & 0x000F:		/* INT ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_LWORD par2 = getLWORD(GetProgramAddress());
				WM_LWORD par3 = getLWORD(GetProgramAddress());
				WM_LWORD wRes = par1 ? par3 : par2;

				setLWORD(wDst, wRes);
			}
			break;

		case VMF_SEL_BOOL_REAL_REAL & 0x000F:		/* REAL ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_REAL par2 = getREAL(GetProgramAddress());
				WM_REAL par3 = getREAL(GetProgramAddress());
				WM_REAL wRes = par1 ? par3 : par2;

				setREAL(wDst, wRes);
			}
			break;

#ifdef VM_LREAL_SUPPORT
		case VMF_SEL_BOOL_LREAL_LREAL & 0x000F:		/* LREAL ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_LREAL par2 = getLREAL(GetProgramAddress());
				WM_LREAL par3 = getLREAL(GetProgramAddress());
				WM_LREAL wRes = par1 ? par3 : par2;

				setLREAL(wDst, wRes);
			}
			break;
#endif
		case VMF_SEL_BOOL_TIME_TIME & 0x000F:		/* INT ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_TIME par2 = getTIME(GetProgramAddress());
				WM_TIME par3 = getTIME(GetProgramAddress());
				WM_TIME wRes = par1 ? par3 : par2;

				setTIME(wDst, wRes);
			}
			break;

		case VMF_SEL_BOOL_DATE_DATE & 0x000F:		/* INT ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_DATE par2;
				WM_DATE par3;
            WM_DATE wRes;
				getDATE(GetProgramAddress(),par2);
				getDATE(GetProgramAddress(),par3);

				wRes = par1 ? par3 : par2;

				setDATE(wDst, wRes);
			}
			break;

		case VMF_SEL_BOOL_TOD_TOD & 0x000F:		/* INT ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_TOD par2;
				WM_TOD par3;
            WM_TOD wRes;

				getTOD(GetProgramAddress(),par2);
				getTOD(GetProgramAddress(),par3);
				wRes = par1 ? par3 : par2;

				setTOD(wDst, wRes);
			}
			break;

		case VMF_SEL_BOOL_DATE_AND_TIME_DATE_AND_TIME & 0x000F:		/* INT ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_DATE_AND_TIME par2;
				WM_DATE_AND_TIME par3;
            WM_DATE_AND_TIME wRes;

				getDATE_AND_TIME(GetProgramAddress(),par2);
				getDATE_AND_TIME(GetProgramAddress(),par3);
				wRes = par1 ? par3 : par2;

				setDATE_AND_TIME(wDst, wRes);
			}
			break;

#ifdef VM_STRING_SUPPORT

		case VMF_SEL_BOOL_STRING_STRING & 0x000F:		/* INT ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_STRING par2;
				WM_STRING par3;
				
				getSTRING(GetProgramAddress(), &par2);
				getSTRING(GetProgramAddress(), &par3);

				if (par1)
					setSTRING(wDst, &par3);
				else
					setSTRING(wDst, &par2);
			}
			break;

#endif

		default:
			bResult = WM_UNKNOWN;
	}
	return;
}
