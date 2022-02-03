/* Bitwise SHIFT and ROTATE implementation */

#define SHIFT_TYPE(type1,type2) \
	case VMF_SHL_##type1##_##type2 & 0x000F: \
	{ \
        WM_##type1 wRes = 0; \
        WM_##type1 par = get##type1(GetProgramAddress()); \
        WM_##type2 wyk = get##type2(GetProgramAddress()); \
        if (opt & 0x10)			/* right or left shift */ \
          wRes = par >> wyk;	\
        else					\
          wRes = par << wyk;	\
		set##type1(wDst, wRes);	\
	} \
	break;

#define ROTATE_TYPE(type1,type2) \
	case VMF_ROL_##type1##_##type2 & 0x000F: \
	{ \
        WM_##type1 wRes = 0; \
        WM_##type1 par = get##type1(GetProgramAddress()); \
        WM_##type2 wyk = get##type2(GetProgramAddress()); \
        if (opt & 0x10)			/* right or left rotate */ \
			wRes = (par >> wyk) | (par << (8*sizeof(par)-wyk));	\
		else													\
			wRes = (par << wyk) | (par >> (8*sizeof(par)-wyk));	\
		set##type1(wDst, wRes);	\
	} \
	break;



void VMCLASS_PREFIX WMC_SHI_ROT(WM_BYTE opt)
{
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */

	if (opt & 0x20)
	{
		switch(opt & 0x0F)
		{

			ROTATE_TYPE(BYTE,INT)
			ROTATE_TYPE(WORD,INT)
			ROTATE_TYPE(DWORD,INT)
			ROTATE_TYPE(LWORD,INT)

			default:
				bResult = WM_UNKNOWN;
		}
	}
	else
	{
		switch(opt & 0x0F)
		{
			SHIFT_TYPE(BYTE,INT)
			SHIFT_TYPE(WORD,INT)
			SHIFT_TYPE(DWORD,INT)
			SHIFT_TYPE(LWORD,INT)

			default:
				bResult = WM_UNKNOWN;
		}
	}
}