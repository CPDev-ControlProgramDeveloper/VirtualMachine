/* EQ and NE implementation */

#define EQ_TYPE(type) \
	case VMF_EQ_##type##_##type & 0x000F: \
	{ \
		WM_##type par1 = get##type(GetProgramAddress()); \
		WM_##type par2 = get##type(GetProgramAddress()); \
		WM_BOOL wRes = par1 == par2; \
		setBOOL(wDst, wRes); \
	} \
	break;

#define NE_TYPE(type) \
	case VMF_EQ_##type##_##type & 0x000F: \
	{ \
		WM_##type par1 = get##type(GetProgramAddress()); \
		WM_##type par2 = get##type(GetProgramAddress()); \
		WM_BOOL wRes = par1 != par2; \
		setBOOL(wDst, wRes); \
	} \
	break;

void VMCLASS_PREFIX WMC_EQ(WM_BYTE opt)
{
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */

	switch(opt & 0x0F)
	{

			EQ_TYPE(SINT);
			EQ_TYPE(INT);
			EQ_TYPE(DINT);
			EQ_TYPE(LINT);
			EQ_TYPE(BYTE);
			EQ_TYPE(WORD);
			EQ_TYPE(DWORD);
			EQ_TYPE(LWORD);
			EQ_TYPE(REAL);
#ifdef VM_LREAL_SUPPORT
			EQ_TYPE(LREAL);
#endif
			EQ_TYPE(TIME);

		case VMF_EQ_BOOL_BOOL & 0x000F:		/* BOOL ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_BOOL par2 = getBOOL(GetProgramAddress());
				WM_BOOL wRes = ((par1!=0) == (par2!=0));

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_EQ_DATE_DATE & 0x000F:		/* INT ops */
			{
				WM_DATE par1;
				WM_DATE par2;
				WM_BOOL wRes;

				getDATE(GetProgramAddress(),par1);
				getDATE(GetProgramAddress(),par2);
				wRes = equDATE(par1,par2);

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_EQ_TIME_OF_DAY_TIME_OF_DAY & 0x000F:		/* INT ops */
			{
				WM_TOD par1;
				WM_TOD par2;
				WM_BOOL wRes;

				getTOD(GetProgramAddress(),par1);
				getTOD(GetProgramAddress(),par2);
				wRes = equTOD(par1,par2);

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_EQ_DATE_AND_TIME_DATE_AND_TIME & 0x000F:		/* INT ops */
			{
				WM_DATE_AND_TIME par1;
				WM_DATE_AND_TIME par2;
				WM_BOOL wRes;

				getDATE_AND_TIME(GetProgramAddress(),par1);
				getDATE_AND_TIME(GetProgramAddress(),par2);
				wRes = equDATE_AND_TIME(par1,par2);

				setBOOL(wDst, wRes);
			}
			break;

#ifdef VM_STRING_SUPPORT
		case VMF_EQ_STRING_STRING & 0x000F:		/* INT ops */
			{
				WM_STRING par1;
				WM_STRING par2;
				getSTRING(GetProgramAddress(), &par1);
				getSTRING(GetProgramAddress(), &par2);
				WM_BOOL wRes = equSTRING(&par1,&par2);

				setBOOL(wDst, wRes);
			}
			break;
#endif

		default:
			bResult = WM_UNKNOWN;
	}
	return;
}

void VMCLASS_PREFIX WMC_NE(WM_BYTE opt)
{
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */

	switch(opt & 0x0F)
	{

			NE_TYPE(SINT);
			NE_TYPE(INT);
			NE_TYPE(DINT);
			NE_TYPE(LINT);
			NE_TYPE(BYTE);
			NE_TYPE(WORD);
			NE_TYPE(DWORD);
			NE_TYPE(LWORD);
			NE_TYPE(REAL);
#ifdef VM_LREAL_SUPPORT
			NE_TYPE(LREAL);
#endif
			NE_TYPE(TIME);

		case VMF_NE_BOOL_BOOL & 0x000F:		/* BOOL ops */
			{
				WM_BOOL par1 = getBOOL(GetProgramAddress());
				WM_BOOL par2 = getBOOL(GetProgramAddress());
				WM_BOOL wRes = ((par1!=0) != (par2!=0));

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_NE_DATE_DATE & 0x000F:		/* INT ops */
			{
				WM_DATE par1;
				WM_DATE par2;
				WM_BOOL wRes;

				getDATE(GetProgramAddress(),par1);
				getDATE(GetProgramAddress(),par2);
				wRes = !equDATE(par1,par2);

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_NE_TIME_OF_DAY_TIME_OF_DAY & 0x000F:		/* INT ops */
			{
				WM_TOD par1;
				WM_TOD par2;
				WM_BOOL wRes;

				getTOD(GetProgramAddress(),par1);
				getTOD(GetProgramAddress(),par2);
				wRes = !equTOD(par1,par2);

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_NE_DATE_AND_TIME_DATE_AND_TIME & 0x000F:		/* INT ops */
			{
				WM_DATE_AND_TIME par1;
				WM_DATE_AND_TIME par2;
				WM_BOOL wRes;
            
				getDATE_AND_TIME(GetProgramAddress(),par1);
				getDATE_AND_TIME(GetProgramAddress(),par2);
				wRes = !equDATE_AND_TIME(par1,par2);

				setBOOL(wDst, wRes);
			}
			break;

#ifdef VM_STRING_SUPPORT
		case VMF_NE_STRING_STRING & 0x000F:		/* INT ops */
			{
				WM_STRING par1;
				WM_STRING par2;
				getSTRING(GetProgramAddress(), &par1);
				getSTRING(GetProgramAddress(), &par2);
				WM_BOOL wRes = !equSTRING(&par1,&par2);

				setBOOL(wDst, wRes);
			}
			break;
#endif

		default:
			bResult = WM_UNKNOWN;
	}
	return;
}
