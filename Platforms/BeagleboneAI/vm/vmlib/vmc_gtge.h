/* GT and GE implementation */

void VMCLASS_PREFIX WMC_GT(WM_BYTE opt)
{
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */

	switch(opt & 0x0F)
	{
		case VMF_GT_BOOL_BOOL & 0x000F:		/* BOOL ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_BOOL par2 = getBOOL(GetProgramAddress());
				WM_BOOL wRes = par1 > par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GT_SINT_SINT & 0x000F:		/* SINT ops */
			{
				WM_SINT par1 = getSINT(GetProgramAddress());
				WM_SINT par2 = getSINT(GetProgramAddress());
				WM_BOOL wRes = par1 > par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GT_INT_INT & 0x000F:		/* INT ops */
			{
				WM_INT par1 = getINT(GetProgramAddress());
				WM_INT par2 = getINT(GetProgramAddress());
				WM_BOOL wRes = par1 > par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GT_DINT_DINT & 0x000F:		/* DINT ops */
			{
				WM_DINT par1 = getDINT(GetProgramAddress());
				WM_DINT par2 = getDINT(GetProgramAddress());
				WM_BOOL wRes = par1 > par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GT_BYTE_BYTE & 0x000F:		/* INT ops */
			{
				WM_BYTE par1 = getBYTE(GetProgramAddress());
				WM_BYTE par2 = getBYTE(GetProgramAddress());
				WM_BOOL wRes = par1 > par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GT_WORD_WORD & 0x000F:		/* INT ops */
			{
				WM_WORD par1 = getWORD(GetProgramAddress());
				WM_WORD par2 = getWORD(GetProgramAddress());
				WM_BOOL wRes = par1 > par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GT_DWORD_DWORD & 0x000F:		/* INT ops */
			{
				WM_DWORD par1 = getDWORD(GetProgramAddress());
				WM_DWORD par2 = getDWORD(GetProgramAddress());
				WM_BOOL wRes = par1 > par2;

				setBOOL(wDst, wRes);
			}
			break;

#ifdef VM_LONG_SUPPORT			

		case VMF_GT_LINT_LINT & 0x000F:		/* LINT ops */
			{
				WM_LINT par1 = getLINT(GetProgramAddress());
				WM_LINT par2 = getLINT(GetProgramAddress());
				WM_BOOL wRes = par1 > par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GT_LWORD_LWORD & 0x000F:		/* INT ops */
			{
				WM_LWORD par1 = getLWORD(GetProgramAddress());
				WM_LWORD par2 = getLWORD(GetProgramAddress());
				WM_BOOL wRes = par1 > par2;

				setBOOL(wDst, wRes);
			}
			break;

#endif

#ifdef VM_REAL_SUPPORT

		case VMF_GT_REAL_REAL & 0x000F:		/* INT ops */
			{
				WM_REAL par1 = getREAL(GetProgramAddress());
				WM_REAL par2 = getREAL(GetProgramAddress());
				WM_BOOL wRes = par1 > par2;

				setBOOL(wDst, wRes);
			}
			break;

#endif			

#ifdef VM_LREAL_SUPPORT
		case VMF_GT_LREAL_LREAL & 0x000F:		/* INT ops */
			{
				WM_LREAL par1 = getLREAL(GetProgramAddress());
				WM_LREAL par2 = getLREAL(GetProgramAddress());
				WM_BOOL wRes = par1 > par2;

				setBOOL(wDst, wRes);
			}
			break;
#endif
		case VMF_GT_TIME_TIME & 0x000F:		/* INT ops */
			{
				WM_TIME par1 = getTIME(GetProgramAddress());
				WM_TIME par2 = getTIME(GetProgramAddress());
				WM_BOOL wRes = par1 > par2;

				setBOOL(wDst, wRes);
			}
			break;

#ifdef VM_DATETIME_SUPPORT

		case VMF_GT_DATE_DATE & 0x000F:		/* INT ops */
			{
				WM_DATE par1;
				WM_DATE par2;
            WM_BOOL wRes;

				getDATE(GetProgramAddress(),par1);
				getDATE(GetProgramAddress(),par2);
				wRes = lessDATE(par2,par1);

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GT_TIME_OF_DAY_TIME_OF_DAY & 0x000F:		/* INT ops */
			{
				WM_TOD par1;
				WM_TOD par2;
            WM_BOOL wRes;

				getTOD(GetProgramAddress(),par1);
				getTOD(GetProgramAddress(),par2);
				wRes = lessTOD(par2,par1);

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GT_DATE_AND_TIME_DATE_AND_TIME & 0x000F:		/* INT ops */
			{
				WM_DATE_AND_TIME par1;
				WM_DATE_AND_TIME par2;
            WM_BOOL wRes;

				getDATE_AND_TIME(GetProgramAddress(),par1);
				getDATE_AND_TIME(GetProgramAddress(),par2);
				wRes = lessDATE_AND_TIME(par2,par1);

				setBOOL(wDst, wRes);
			}
			break;

#endif			

#ifdef VM_STRING_SUPPORT
		case VMF_GT_STRING_STRING & 0x000F:		/* INT ops */
			{
				WM_STRING par1;
				WM_STRING par2;
				getSTRING(GetProgramAddress(), &par1);
				getSTRING(GetProgramAddress(), &par2);
				WM_BOOL wRes = lessSTRING(&par2,&par1);

				setBOOL(wDst, wRes);
			}
			break;
#endif
		default:
			bResult = WM_UNKNOWN;
	}
	return;
}

void VMCLASS_PREFIX WMC_GE(WM_BYTE opt)
{
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */

	switch(opt & 0x0F)
	{
		case VMF_GE_BOOL_BOOL & 0x000F:		/* BOOL ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_BOOL par2 = getBOOL(GetProgramAddress());
				WM_BOOL wRes = par1 >= par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GE_SINT_SINT & 0x000F:		/* SINT ops */
			{
				WM_SINT par1 = getSINT(GetProgramAddress());
				WM_SINT par2 = getSINT(GetProgramAddress());
				WM_BOOL wRes = par1 >= par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GE_INT_INT & 0x000F:		/* INT ops */
			{
				WM_INT par1 = getINT(GetProgramAddress());
				WM_INT par2 = getINT(GetProgramAddress());
				WM_BOOL wRes = par1 >= par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GE_DINT_DINT & 0x000F:		/* DINT ops */
			{
				WM_DINT par1 = getDINT(GetProgramAddress());
				WM_DINT par2 = getDINT(GetProgramAddress());
				WM_BOOL wRes = par1 >= par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GE_BYTE_BYTE & 0x000F:		/* INT ops */
			{
				WM_BYTE par1 = getBYTE(GetProgramAddress());
				WM_BYTE par2 = getBYTE(GetProgramAddress());
				WM_BOOL wRes = par1 >= par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GE_WORD_WORD & 0x000F:		/* INT ops */
			{
				WM_WORD par1 = getWORD(GetProgramAddress());
				WM_WORD par2 = getWORD(GetProgramAddress());
				WM_BOOL wRes = par1 >= par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GE_DWORD_DWORD & 0x000F:		/* INT ops */
			{
				WM_DWORD par1 = getDWORD(GetProgramAddress());
				WM_DWORD par2 = getDWORD(GetProgramAddress());
				WM_BOOL wRes = par1 >= par2;

				setBOOL(wDst, wRes);
			}
			break;

#ifdef VM_LONG_SUPPORT
		
		case VMF_GE_LINT_LINT & 0x000F:		/* LINT ops */
			{
				WM_LINT par1 = getLINT(GetProgramAddress());
				WM_LINT par2 = getLINT(GetProgramAddress());
				WM_BOOL wRes = par1 >= par2;

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GE_LWORD_LWORD & 0x000F:		/* INT ops */
			{
				WM_LWORD par1 = getLWORD(GetProgramAddress());
				WM_LWORD par2 = getLWORD(GetProgramAddress());
				WM_BOOL wRes = par1 >= par2;

				setBOOL(wDst, wRes);
			}
			break;
#endif			

#ifdef VM_REAL_SUPPORT

		case VMF_GE_REAL_REAL & 0x000F:		/* INT ops */
			{
				WM_REAL par1 = getREAL(GetProgramAddress());
				WM_REAL par2 = getREAL(GetProgramAddress());
				WM_BOOL wRes = par1 >= par2;

				setBOOL(wDst, wRes);
			}
			break;
#endif


#ifdef VM_LREAL_SUPPORT
		case VMF_GE_LREAL_LREAL & 0x000F:		/* LREAL ops */
			{
				WM_LREAL par1 = getLREAL(GetProgramAddress());
				WM_LREAL par2 = getLREAL(GetProgramAddress());
				WM_BOOL wRes = par1 >= par2;

				setBOOL(wDst, wRes);
			}
			break;
#endif
		case VMF_GE_TIME_TIME & 0x000F:		/* INT ops */
			{
				WM_TIME par1 = getTIME(GetProgramAddress());
				WM_TIME par2 = getTIME(GetProgramAddress());
				WM_BOOL wRes = par1 >= par2;

				setBOOL(wDst, wRes);
			}
			break;

#ifdef VM_DATETIME_SUPPORT			

		case VMF_GE_DATE_DATE & 0x000F:		/* INT ops */
			{
				WM_DATE par1;
				WM_DATE par2;
            WM_BOOL wRes;

				getDATE(GetProgramAddress(),par1);
				getDATE(GetProgramAddress(),par2);
				wRes = !lessDATE(par1,par2);

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GE_TIME_OF_DAY_TIME_OF_DAY & 0x000F:		/* INT ops */
			{
				WM_TOD par1;
				WM_TOD par2;
            WM_BOOL wRes;

				getTOD(GetProgramAddress(),par1);
				getTOD(GetProgramAddress(),par2);
				wRes = !lessTOD(par1,par2);

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_GE_DATE_AND_TIME_DATE_AND_TIME & 0x000F:		/* INT ops */
			{
				WM_DATE_AND_TIME par1;
				WM_DATE_AND_TIME par2;
            WM_BOOL wRes;

				getDATE_AND_TIME(GetProgramAddress(),par1);
				getDATE_AND_TIME(GetProgramAddress(),par2);
				wRes = !lessDATE_AND_TIME(par1,par2);

				setBOOL(wDst, wRes);
			}
			break;

#endif			

#ifdef VM_STRING_SUPPORT
		case VMF_GE_STRING_STRING & 0x000F:		/* INT ops */
			{
				WM_STRING par1;
				WM_STRING par2;
				getSTRING(GetProgramAddress(), &par1);
				getSTRING(GetProgramAddress(), &par2);
				WM_BOOL wRes = !lessSTRING(&par1,&par2);

				setBOOL(wDst, wRes);
			}
			break;
#endif

		default:
			bResult = WM_UNKNOWN;
	}
	return;
}
