#ifndef INC_VMREGISTERS_H
#define INC_VMREGISTERS_H

	WM_ADDRESS wCallingStack[CALL_STACK_SIZE];		// function block calling stack
	WM_BYTE wCallingStackPtr;				// and its pointer
	WM_ADDRESS wDataOfsStack[DATA_STACK_SIZE];		// data offset stack
	WM_BYTE wDataOfsStackPtr;				// and its pointer

	WM_BYTE bRunMode;

	WM_WORD wStatus1; //0x4;					// bit 0 - przekroczono cykl zadania

	WM_ADDRESS wProgramCounter;			// program counter register (CSIP)
	WM_ADDRESS wDataOfs;				// data pointer register (DPTR)

	unsigned long nCycles;				/* count of program cycles performed */

	WM_WORD task_cycle;		/* task cycle (ms) */

	WM_DATE_AND_TIME wm_rtc_value;	/* czas astronomiczny odczytywany na pocz¹tku cyklu */

	WM_BYTE *pgmCode;		// main pointer to the VM program area

	WM_ADDRESS cycle_start_adr;	//address of first instruction of a cycle

	WM_BYTE *pgmData;

	WM_BYTE	bResult;		// command execution flag

#endif /* INC_VMREGISTERS_H */