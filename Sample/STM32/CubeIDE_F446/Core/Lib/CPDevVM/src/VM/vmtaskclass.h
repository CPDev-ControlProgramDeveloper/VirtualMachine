#ifndef INC_VMTASKCLASS_H
#define INC_VMTASKCLASS_H

#define VMTASK_CLASS

#include "vm.h"

class VMCLASS_NAME
{
public:	// common functions

	VMCLASS_NAME() { pgmData = 0;
					pgmCode = 0;
					task_cycle = 200;}

	int WM_Run(int mode);
	int WM_Stop(void);

	void WM_Initialize(int mode);
	void WM_RunCycle();
	void WM_Shutdown();

	int WM_GetData(WM_ADDRESS address, WM_BYTE len, WM_BYTE* buf);
	int WM_SetData(WM_ADDRESS address, WM_BYTE len, WM_BYTE* buf);

	unsigned long WM_GetCycles(void);
	WM_WORD WM_GetStatus1(void);

	// platform-dependent functions (to be implemented in derived class)
public:
	/// <summary>
	/// Set up machine memory for code and data.
	/// Allocate data memory and set pgmData pointer accoringly.
	/// Fill code memory with xcp code and set pgmCode pointer accordingly.
	/// </summary>
	/// <param name="file">Optional XCP file name to load</param>
	/// <param name="datasize">Optional program data size (as stored in DCP)</param>
	/// <returns>0 - success, -1 - error, cannot load</returns>
	virtual int VMP_LoadProgramAndData(const wchar_t* file, int datasize = DEFAULT_DATA_SIZE) = 0;
protected:

	virtual void VMP_PostCycle(void) {};
	virtual void VMP_PreCycle(void) {};
	virtual void VMP_Debug(WM_BYTE err, WM_WORD aux) {};
	virtual void VMP_PreNextCommand(void) {};

	/// <summary>
	/// Called when a program is started. Perform some initialization here.
	/// <example>srand((unsigned)time(NULL));</example> 
	/// </summary>
	virtual void VMP_PreProgram(void) {};

	/// <summary>
	/// Called after the program has been stopped for a reason.
	/// Implement to free resources, etc.
	/// </summary>
	virtual void VMP_PostProgram(void) {};

	virtual WM_BYTE VMP_FlashDataRead(WM_WORD StartAddress, WM_WORD Size) { return 0; };
	virtual WM_BYTE VMP_FlashDataWrite(WM_WORD StartAddress, WM_WORD Size) { return 0; };
	virtual WM_WORD VMP_FlashDataControl(WM_WORD Command) { return 5; };

	/// <summary>
	/// Returns current system clock in milliseconds.
	/// <example>GetTickCount64()</example>
	/// </summary>
	/// <returns></returns>
	virtual WM_TIME VMP_CurrentTime() = 0;

	/// <summary>
	/// Sets the given dt to current date and time
	/// <example>localtime()</example>
	/// </summary>
	/// <param name="dt">DT variable to set</param>
	virtual void VMP_ReadRTC(WM_DATE_AND_TIME *dt)
		{ dt->date.date.year = dt->date.date.month = dt->date.date.day = dt->tod.tod.hour = dt->tod.tod.min = dt->tod.tod.sec = dt->tod.tod.csec = 0; };

	/// <summary>
	/// Sets the platorm-specific RTC setting to the given dt
	/// Implement if RTC may be changable via IEC 61131-3 software
	/// </summary>
	virtual WM_BOOL VMP_WriteRTC(WM_DATE_AND_TIME *dt) { return FALSE; };

	/// <summary>
	/// Returns a random number.
	/// <example>return (WM_REAL)((WM_REAL)rand() / RAND_MAX);</example>
	/// </summary>
	virtual WM_REAL VMP_GetRandom() = 0; 

	virtual int VMP_ExecNativeBlock(WM_WORD fb_selector, WM_ADDRESS fb_instance) { return 2; };
	virtual int VMP_InitNativeBlock(WM_WORD fb_selector, WM_ADDRESS fb_instance) { return 2; };

protected:
	void WM_InitRegisters(void);
	int WM_RunCommand(void);

	// data access
	WM_WORD GetProgramWord(void);
	WM_DWORD GetProgramDWord(void);
	WM_ADDRESS GetProgramAddress(void);

	WM_REAL getREAL(WM_ADDRESS x);
	void setREAL(WM_ADDRESS x, WM_REAL val);
#ifdef VM_LREAL_SUPPORT
	WM_LREAL getLREAL(WM_ADDRESS x);
	void setLREAL(WM_ADDRESS x, WM_LREAL val);
#endif
	WM_INT getINT(WM_ADDRESS x);
	void setINT(WM_ADDRESS x, WM_INT val);
	WM_DINT getDINT(WM_ADDRESS x);
	void setDINT(WM_ADDRESS x, WM_DINT val);
	WM_LINT getLINT(WM_ADDRESS x);
	void setLINT(WM_ADDRESS x, WM_LINT val);
#ifdef VM_STRING_SUPPORT
	void getSTRING(WM_ADDRESS src, WM_STRING* dst);  /* get string variable */
	void setSTRING(WM_ADDRESS dst, WM_STRING* src);	/* set string variable */
#endif

	// instruction implementation
	void WMC_ADD(WM_BYTE opt);
	void WMC_SYSCTRL(WM_BYTE opt);
	WM_BYTE WMC_Custom(WM_BYTE opcode);
	void WMC_Pointers(WM_BYTE opt);
	void WMC_MMV(WM_BYTE opt);
	void WMC_SUB(WM_BYTE opt);
	void WMC_MUL(WM_BYTE opt);
	void WMC_DIV_MOD(WM_BYTE opt);
	void WMC_NEG_NOT(WM_BYTE opt);
#ifdef VM_MATH_SUPPORT
	void WMC_EXPT(WM_BYTE opt);
	void WMC_ABS(WM_BYTE opt);
	void WMC_MATH(WM_BYTE opt);
	void WMC_EXPT_ABS(WM_BYTE opt);
#endif
	void WMC_AND(WM_BYTE opt);
	void WMC_OR(WM_BYTE opt);
	void WMC_XOR(WM_BYTE opt);
	void WMC_SHI_ROT(WM_BYTE opt);
	void WMC_SEL(WM_BYTE opt);
	void WMC_LIMIT(WM_BYTE opt);
	void WMC_MAX(WM_BYTE opt);
	void WMC_MIN(WM_BYTE opt);
	void WMC_GT(WM_BYTE opt);
	void WMC_GE(WM_BYTE opt);
	void WMC_LT(WM_BYTE opt);
	void WMC_LE(WM_BYTE opt);
	void WMC_EQ(WM_BYTE opt);
	void WMC_NE(WM_BYTE opt);
	void WMC_MUX(WM_BYTE opt);
	void WMC_CONV(WM_BYTE opt);
	void WMC_DOW(WM_BYTE opt);
#ifdef VM_STRING_SUPPORT
	void WMC_String(WM_BYTE opt);
#endif

protected:
	/* stack functions */
	void push_DataOfsStack(WM_ADDRESS w);
	WM_ADDRESS pull_DataOfsStack(void);
	void push_CallingStack(WM_ADDRESS w);
	WM_ADDRESS pull_CallingStack(void);
	void clear_Stacks(void);

public:

#include "vmregisters.h"


};

#endif /* INC_VMTASKCLASS_H */