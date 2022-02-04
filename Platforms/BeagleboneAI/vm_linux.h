// enable loading XCP from SD card (not for UNO, too large)
#define USE_SD_XCP

#ifndef VM_LINUX_H
#define VM_LINUX_H

#ifndef VMTASK_CLASS
#define VMTASK_CLASS
#endif

#if defined(__arm__)
#define BYTE_ACCESS
#endif

#include "vm/vmtaskclass.h"
#include "vm_variable.h"

class VMLinux : public VMCLASS_NAME
{
public:
	int VMP_LoadProgramAndData(const char* file, int datasize = DEFAULT_DATA_SIZE);
	int VMP_LoadProgramFromArray(const unsigned char* code, int datasize = DEFAULT_DATA_SIZE);
	using VMCLASS_NAME::WM_GetData;
	int WM_GetData(VMVariable* var, WM_BYTE* buf);
	WM_BYTE WM_GetDataByte(VMVariable* var);
	using VMCLASS_NAME::WM_SetData;
	int WM_SetData(VMVariable* var, WM_BYTE* buf);
	int WM_SetDataByte(VMVariable* var, WM_BYTE value);

	unsigned long long int calcTime;			// calculation time for testing

private:
	void VMP_PostCycle(void);
	void VMP_PreCycle(void);
	void VMP_Debug(WM_BYTE err, WM_WORD aux);
	void VMP_PreProgram(void);
	void VMP_PreNextCommand();

	WM_TIME VMP_CurrentTime();

	void VMP_ReadRTC(WM_DATE_AND_TIME *dt);

	WM_REAL VMP_GetRandom();

	unsigned long long int cStartTime;			// cycle starting time (for constant cycle calculation)

	WM_BYTE pgmDataDefault[DEFAULT_DATA_SIZE];
};

#endif