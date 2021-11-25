/* Memory instructions */

void VMCLASS_PREFIX WMC_MMV(WM_BYTE opt)
{
	WM_ADDRESS wPar1, wPar2;

	wPar1 = GetProgramAddress();
	wPar2 = GetProgramAddress();

#ifdef VM_CUSTOM_GLOBAL

	VMP_MemoryMove(wPar1, wPar2, opt);

#else

	if (opt & 0x80)		// Relative to Global
		wPar2 += wDataOfs; //-- by³o wDataOffsetReg
	else
		wPar1 += wDataOfs; //-- by³o wDataOffsetReg

	memcpy(pgmData + wPar1, pgmData + wPar2, opt & 0x7F);

#endif

	return;
}
