extern int VMP_LoadProgramAndData(const char* filename, int datasize);	// allocate program and data memory areas, load a program, return -1 on error
extern void VMP_PreCycle(void);			// called at the start of a cycle
extern void VMP_PostCycle(void);		// called after a cycle finishes
extern void VMP_PreNextCommand(void);		// called before fetching next instruction
extern void VMP_PreProgram(void);		// called before a program starts
extern void VMP_PostProgram(void);		// called after a program finishes
extern void VMP_Debug(WM_BYTE err, WM_WORD aux);		// called on errors and exeptions
extern void VMP_ReadRTC(WM_DATE_AND_TIME *dt);		// to get the real-time clock value
extern WM_BOOL VMP_WriteRTC(WM_DATE_AND_TIME *dt);		// to set the real-time clock value
extern WM_TIME VMP_CurrentTime(void);		// to get the system clock
extern WM_REAL VMP_GetRandom(void);		// to generate a random number

extern int VMP_ExecNativeBlock(WM_WORD fb_selector, WM_ADDRESS fb_instance);	// to execute a native block
extern int VMP_InitNativeBlock(WM_WORD fb_selector, WM_ADDRESS fb_instance);	// to initialize a native block

extern WM_BYTE VMP_FlashDataRead(WM_ADDRESS StartAddress, WM_WORD Size);
extern WM_BYTE VMP_FlashDataWrite(WM_ADDRESS StartAddress, WM_WORD Size);
extern WM_WORD VMP_FlashDataControl(WM_WORD Command);

#ifdef ASSERT_ON
extern void VMP_Assert(WM_BYTE assert_type, WM_WORD error_code);
#endif

#ifdef VM_CUSTOM_GLOBAL
extern void VMP_MemoryMove(WM_ADDRESS from, WM_ADDRESS to, WM_BYTE option_byte);
#endif