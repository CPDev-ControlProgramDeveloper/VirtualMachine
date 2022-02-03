/* SUB implementation */

#define SUB_TYPE(type1,type2) \
	case VMF_SUB_##type1##_##type2 & 0x000F: \
	{ \
	WM_##type1 wRes = get##type1(GetProgramAddress()) - get##type2(GetProgramAddress()); \
	set##type1(wDst, wRes); } \
	break;

void VMCLASS_PREFIX WMC_SUB(WM_BYTE opt)
{
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */

	switch(opt & 0x0F)
	{
		SUB_TYPE(SINT,SINT)
		SUB_TYPE(INT,INT)
		SUB_TYPE(DINT,DINT)
		SUB_TYPE(LINT,LINT)
		SUB_TYPE(BYTE,BYTE)
		SUB_TYPE(WORD,WORD)
		SUB_TYPE(DWORD,DWORD)
		SUB_TYPE(TIME,TIME)
		SUB_TYPE(LWORD,LWORD)
		SUB_TYPE(REAL,REAL)
#ifdef VM_LREAL_SUPPORT
		SUB_TYPE(LREAL,LREAL)
#endif
		default:
			bResult = WM_UNKNOWN;
	}
	return;
}
