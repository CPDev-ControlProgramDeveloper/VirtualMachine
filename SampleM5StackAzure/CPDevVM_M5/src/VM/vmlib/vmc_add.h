/* ADD implementation */

#define ADD_TYPE(type) \
	case VMF_ADD_##type & 0x000F: \
	{ \
		WM_##type wRes = 0; \
		for (i=0; i<bCnt; i++) \
			wRes += get##type(GetProgramAddress()); \
		set##type(wDst, wRes); \
	} \
	break;

void VMCLASS_PREFIX WMC_ADD(WM_BYTE opt)
{
	WM_BYTE bCnt = opt >> 4;	/* parameter count */
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */
	WM_BYTE i;

	switch(opt & 0x0F)
	{

		ADD_TYPE(SINT);
		ADD_TYPE(INT);
		ADD_TYPE(DINT);
		ADD_TYPE(TIME);
		ADD_TYPE(LINT);
		ADD_TYPE(BYTE);
		ADD_TYPE(WORD);
		ADD_TYPE(DWORD);
		ADD_TYPE(LWORD);
		ADD_TYPE(REAL);
#ifdef VM_LREAL_SUPPORT
		ADD_TYPE(LREAL);
#endif

#ifdef VM_STRING_SUPPORT

		case VMF_ADD_STRING & 0x000F:
		{
			WM_STRING wRes, wTmp;
			getSTRING(GetProgramAddress(), &wRes);

			if (--bCnt)
			{
				do {
					getSTRING(GetProgramAddress(), &wTmp);
					copySTRING(&wRes, wRes.length, &wTmp, 0, wTmp.length);
				} while (--bCnt);
			}

			setSTRING(wDst, &wRes);
			break;
		}

#endif

		default:
			bResult = WM_UNKNOWN;
	}
	return;
}
