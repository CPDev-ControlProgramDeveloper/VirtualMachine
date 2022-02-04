/* MUL implementation */

#define MUL_TYPE(type) \
	case VMF_MUL_##type & 0x000F: \
	{ \
		WM_##type wRes = 1; \
		for (i=0; i<bCnt; i++) \
			wRes *= get##type(GetProgramAddress()); \
		set##type(wDst, wRes); \
	} \
	break;


void VMCLASS_PREFIX WMC_MUL(WM_BYTE opt)
{
	WM_BYTE bCnt = opt >> 4;	/* parameter count */
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */
	WM_BYTE i;

	switch(opt & 0x0F)
	{
		MUL_TYPE(SINT)
		MUL_TYPE(INT)
		MUL_TYPE(DINT)
		MUL_TYPE(BYTE)
		MUL_TYPE(WORD)
		MUL_TYPE(DWORD)
#ifdef VM_LONG_SUPPORT			
		MUL_TYPE(LWORD)
		MUL_TYPE(LINT)
#endif
#ifdef VM_REAL_SUPPORT		
		MUL_TYPE(REAL)
#endif		
#ifdef VM_LREAL_SUPPORT
		MUL_TYPE(LREAL)
#endif
		default:
			bResult = WM_UNKNOWN;
	}
	return;
}

