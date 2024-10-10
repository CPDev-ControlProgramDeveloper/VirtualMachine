/* NEG and NOT implementation */

#define NEG_TYPE(type) \
	case VMF_NEG_##type & 0x000F: \
	{ \
	    WM_##type wRes =  -get##type(GetProgramAddress()); \
		set##type(wDst, wRes); \
	} \
	break;

#define NOT_TYPE(type) \
	case VMF_NOT_##type & 0x000F: \
	{ \
	    WM_##type wRes =  ~get##type(GetProgramAddress()); \
		set##type(wDst, wRes); \
	} \
	break;


void VMCLASS_PREFIX WMC_NEG_NOT(WM_BYTE opt)
{
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */

	if (opt & 0x10)
	{
		switch(opt & 0x0F)
		{
	
			case VMF_NOT_BOOL & 0x000F:		/* BOOL ops - special case*/
				{
					WM_BOOL wRes = !getBYTE(GetProgramAddress());
					setBOOL(wDst, wRes);
				}
				break;

			NOT_TYPE(BYTE)
			NOT_TYPE(WORD)
			NOT_TYPE(DWORD)
#ifdef VM_LONG_SUPPORT				
			NOT_TYPE(LWORD)
#endif			

			default:
				bResult = WM_UNKNOWN;
		}
	}
	else
	{
		switch(opt & 0x0F)
		{

			NEG_TYPE(SINT)
			NEG_TYPE(INT)
			NEG_TYPE(DINT)
#ifdef VM_LONG_SUPPORT				
			NEG_TYPE(LINT)
#endif
#ifdef VM_REAL_SUPPORT			
			NEG_TYPE(REAL)
#endif			
#ifdef VM_LREAL_SUPPORT
			NEG_TYPE(LREAL)
#endif
			default:
				bResult = WM_UNKNOWN;
		}
	}
}
