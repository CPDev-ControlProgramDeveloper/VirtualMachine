/* OR implementation */


#define OR_TYPE(type) \
	case VMF_OR_##type & 0x000F: \
	{ \
		WM_##type wRes = 0; \
		for (i=0; i<bCnt; i++) \
			wRes |= get##type(GetProgramAddress()); \
		set##type(wDst, wRes); \
	} \
	break;


void VMCLASS_PREFIX WMC_OR(WM_BYTE opt)
{
	WM_BYTE bCnt = opt >> 4;	/* parameter count */
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */
	WM_BYTE i;

	switch(opt & 0x0F)
	{
		OR_TYPE(BOOL);
		OR_TYPE(BYTE);
		OR_TYPE(WORD);
		OR_TYPE(DWORD);
		OR_TYPE(LWORD);

		default:
			bResult = WM_UNKNOWN;
	}
	return;
}
