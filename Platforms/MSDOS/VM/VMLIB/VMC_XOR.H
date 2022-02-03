/* XOR implementation */

#define XOR_TYPE(type) \
	case VMF_XOR_##type & 0x000F: \
	{ \
		WM_##type wRes = get##type(GetProgramAddress()); \
		for (i=0; i<bCnt; i++) \
			wRes ^= get##type(GetProgramAddress()); \
		set##type(wDst, wRes); \
	} \
	break;

void VMCLASS_PREFIX WMC_XOR(WM_BYTE opt)
{
	WM_BYTE bCnt = opt >> 4;	/* parameter count */
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */
	WM_BYTE i;

	bCnt--;

	switch(opt & 0x0F)
	{

		XOR_TYPE(BOOL)
		XOR_TYPE(BYTE)
		XOR_TYPE(WORD)
		XOR_TYPE(DWORD)
		XOR_TYPE(LWORD)

		default:
			bResult = WM_UNKNOWN;
	}
	return;
}
