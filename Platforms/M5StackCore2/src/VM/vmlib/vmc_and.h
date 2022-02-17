/* AND implementartion */

void VMCLASS_PREFIX WMC_AND(WM_BYTE opt)
{
	WM_BYTE bCnt = opt >> 4;	/* parameter count */
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */
	WM_BYTE i;

	switch(opt & 0x0F)
	{
		case VMF_AND_BOOL & 0x000F:		/* BOOL ops */
			{
				WM_BOOL wRes = 0x01;

				for (i=0; i<bCnt; i++)
					wRes &= getBOOL(GetProgramAddress());

				setBOOL(wDst, wRes);
			}
			break;

		case VMF_AND_BYTE & 0x000F:		/* BYTE ops */
			{
				WM_BYTE wRes = 0xFF;

				for (i=0; i<bCnt; i++)
					wRes &= getBYTE(GetProgramAddress());

				setBYTE(wDst, wRes);
			}
			break;

		case VMF_AND_WORD & 0x000F:		/* WORD ops */
			{
				WM_WORD wRes = 0xFFFF;

				for (i=0; i<bCnt; i++)
					wRes &= getWORD(GetProgramAddress());

				setWORD(wDst, wRes);
			}
			break;

		case VMF_AND_DWORD & 0x000F:		/* DWORD ops */
			{
				WM_DWORD wRes = 0xFFFFFFFF;

				for (i=0; i<bCnt; i++)
					wRes &= getDWORD(GetProgramAddress());

				setDWORD(wDst, wRes);
			}
			break;

		case VMF_AND_LWORD & 0x000F:		/* LWORD ops */
			{
				WM_LWORD wRes = 0xFFFFFFFFFFFFFFFF;

				for (i=0; i<bCnt; i++)
					wRes &= getLWORD(GetProgramAddress());

				setLWORD(wDst, wRes);
			}
			break;

		default:
			bResult = WM_UNKNOWN;
	}
	return;
}
