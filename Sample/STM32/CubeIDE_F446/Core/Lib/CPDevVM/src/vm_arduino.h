// enable loading XCP from SD card (not for UNO, too large)
//#define USE_SD_XCP

#ifdef ARDUINO_AVR_UNO
#ifdef USE_SD_XCP
#undef USE_SD_XCP
#error SD support is too much for UNO ! Disable it in vm_arduino.h
#endif
#endif

#ifndef VM_ARDUINO_H
#define VM_ARDUINO_H

#ifndef VMTASK_CLASS
#define VMTASK_CLASS
#endif

#include "VM\vmtaskclass.h"

class VMArduino : public VMCLASS_NAME
{
public:
	int VMP_LoadProgramAndData(const wchar_t* file, int datasize = DEFAULT_DATA_SIZE);
	int VMP_LoadProgramFromArray(const unsigned char* code, int datasize = DEFAULT_DATA_SIZE);

private:
	void VMP_PostCycle(void);
	void VMP_PreCycle(void);
	void VMP_Debug(WM_BYTE err, WM_WORD aux);
	void VMP_PreProgram(void);
	void VMP_PreNextCommand();

	WM_TIME VMP_CurrentTime();

	void VMP_ReadRTC(WM_DATE_AND_TIME *dt);

	WM_REAL VMP_GetRandom();

	unsigned long long cStartTimeMs;			// cycle starting time in ms (for constant cycle calculation)
	unsigned long long cStartTimeUs;			// cycle starting time in us (for testing)
	unsigned long cPeroidTimeUs;			// cycle peroid in us (for testing)
	unsigned long cPeroidTimeMs;			// cycle peroid in ms (for testing)

	WM_BYTE pgmDataDefault[DEFAULT_DATA_SIZE];
};

#endif
