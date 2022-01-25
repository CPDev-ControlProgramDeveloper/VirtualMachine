/* DIV and MOD implementation */

#define DIV_TYPE(type1,type2) \
	case VMF_DIV_##type1##_##type2 & 0x000F: \
	{ \
		WM_##type1 wRes = get##type1(GetProgramAddress()) / get##type2(GetProgramAddress()); \
		set##type1(wDst, wRes); \
	} \
	break;


#define MOD_TYPE(type1,type2) \
	case VMF_MOD_##type1##_##type2 & 0x000F: \
	{ \
		WM_##type1 wRes = get##type1(GetProgramAddress()) % get##type2(GetProgramAddress()); \
		set##type1(wDst, wRes); \
	} \
	break;


void VMCLASS_PREFIX WMC_DIV_MOD(WM_BYTE opt)
{
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */

	if (opt & 0x10)	// MOD
	{
		switch(opt & 0x0F)
		{

			MOD_TYPE(SINT,SINT)
			MOD_TYPE(INT,INT)
			MOD_TYPE(DINT,DINT)
			MOD_TYPE(LINT,LINT)
			MOD_TYPE(BYTE,BYTE)
			MOD_TYPE(WORD,WORD)
			MOD_TYPE(DWORD,DWORD)
			MOD_TYPE(LWORD,LWORD)

			default:
				bResult = WM_UNKNOWN;
		}

	}
	else // DIV
	{
		switch(opt & 0x0F)
		{
			DIV_TYPE(SINT,SINT)
			DIV_TYPE(INT,INT)
			DIV_TYPE(DINT,DINT)
			DIV_TYPE(LINT,LINT)
			DIV_TYPE(BYTE,BYTE)
			DIV_TYPE(WORD,WORD)
			DIV_TYPE(DWORD,DWORD)
			DIV_TYPE(LWORD,LWORD)
			DIV_TYPE(REAL,REAL)
#ifdef VM_LREAL_SUPPORT
			DIV_TYPE(LREAL,LREAL)
#endif
			default:
				bResult = WM_UNKNOWN;
		}
	}
}


