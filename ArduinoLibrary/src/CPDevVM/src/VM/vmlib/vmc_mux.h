/* MUX implementation */

#define MUX_TYPE(type) \
	case VMF_MUX_INT_##type & 0x000F: \
	{ \
		WM_##type wRes = 0; \
		for (i=0; i<bCnt; i++) { \
			wRes = get##type(GetProgramAddress()); \
			if (i==par1) set##type(wDst, wRes); } \
	} \
	break;

#define MUX_TYPE2(type) \
	case VMF_MUX_INT_##type & 0x000F: \
	{ \
		WM_##type wRes = {{0}}; \
		for (i=0; i<bCnt; i++) { \
			get##type(GetProgramAddress(), wRes); \
			if (i==par1) set##type(wDst, wRes); } \
	} \
	break;


void VMCLASS_PREFIX WMC_MUX(WM_BYTE opt)
{
	WM_BYTE bCnt = (opt >> 4) - 1;	/* parameter count (excluding selector) */
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */
	WM_BYTE i;//, s=0;

	WM_INT par1 = getINT(GetProgramAddress());

	switch(opt & 0x0F)
	{
		MUX_TYPE(BOOL);
		MUX_TYPE(SINT);
		MUX_TYPE(INT);
		MUX_TYPE(DINT);
		MUX_TYPE(LINT);
		MUX_TYPE(BYTE);
		MUX_TYPE(WORD);
		MUX_TYPE(DWORD);
		MUX_TYPE(LWORD);
		MUX_TYPE(REAL);
#ifdef VM_LREAL_SUPPORT
		MUX_TYPE(LREAL);
#endif
		MUX_TYPE(TIME);
		MUX_TYPE2(DATE);
		MUX_TYPE2(TIME_OF_DAY);
		MUX_TYPE2(DATE_AND_TIME);

#ifdef VM_STRING_SUPPORT
		case VMF_MUX_INT_STRING & 0x000F:
		{
			WM_STRING wRes;
			for (i=0; i<bCnt; i++) {
				getSTRING(GetProgramAddress(), &wRes);
				if (i==par1) setSTRING(wDst, &wRes); }
			break;
		}
#endif
		default:
			bResult = WM_UNKNOWN;
	}
	return;
}
