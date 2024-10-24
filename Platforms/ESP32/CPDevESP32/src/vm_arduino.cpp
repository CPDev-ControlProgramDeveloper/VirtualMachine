#include "vm_arduino.h"

#ifdef USE_SD_XCP

#include <string.h>
#include <stdlib.h>

size_t wchar2char (char *s, const wchar_t *pwcs, size_t n)
{
  int count = 0;
  
  if (n != 0) {
    do {
      if ((*s++ = (char) *pwcs++) == 0)
	break;
      count++;
    } while (--n != 0);
  }
  
  return count;
}
#endif

int VMArduino::VMP_LoadProgramAndData(const char* file, int datasize)
{
#ifdef USE_SD_XCP
	long len;

	//SdFat SD;

	//if (!SD.begin()) {
	//	Serial.println("SD init failed!");
	//	while (1);
	//}
	//Serial.println("SD init ok.");

	// open binary file
    //char cfile[32];
    //wchar2char ( cfile, file, sizeof(cfile) );

    Serial.print("Loading: ");
    Serial.println(file);

	File fileXCP = SD.open(file, FILE_READ);

	// if the file opened okay
	if (!fileXCP)
		return -1;

	//check size of the code
	len = fileXCP.size();
	if (len == -1) return -2;

	// allocate memory for the code
	pgmCode = (WM_BYTE*)malloc(len);
	if (!pgmCode) return -3;

	// read the code into the allocated memory
	fileXCP.read(pgmCode, len);

	fileXCP.close();

	// first release data memory if it has been previously allocated
	if (pgmData) free(pgmData);

	// if data size parameter is 0, use default data memory size
	if (!datasize) datasize = DEFAULT_DATA_SIZE;

	// allocate data memory and clear it
	pgmData = (WM_BYTE*)malloc(datasize);
	if (!pgmData) return -4;
	memset(pgmData, 0, datasize);

	// set initial status of the machine
	wStatus1 = WMSTAT_COLDRESTART;

	// clear cycle counter
	nCycles = 0;

	return 0;	// success
#else
	Serial.println("SD not available !");
	return -1;

#endif
}

int VMArduino::VMP_LoadProgramFromArray(const unsigned char* code, int datasize)
{
	if (datasize > sizeof(pgmDataDefault))
		return -4;

  	pgmCode = (WM_BYTE*) code;
  
	pgmData = pgmDataDefault;

	// set initial status of the machine
	wStatus1 = WMSTAT_COLDRESTART;

	// clear cycle counter
	nCycles = 0;

	Serial.println("Code set to array");

	return 0;	// success
}

void VMArduino::VMP_Debug(WM_BYTE err, WM_WORD aux)
{
  Serial.print("Runtime error ");
  Serial.print(err, aux);  
  Serial.print(" near ");
  Serial.println(wProgramCounter);  
	bRunMode = 0;
}

void VMArduino::VMP_PreCycle(void)
{
	cStartTime = millis();
}

void VMArduino::VMP_PostCycle(void)
{
	WM_LWORD stopTime, cycleEnd;

	if (task_cycle == 0)
		return;

	stopTime = millis();

	cycleEnd = cStartTime + task_cycle;

	if (stopTime > cycleEnd)
	{
		wStatus1 |= WMSTAT_CYCLEOVERRUN;
		return;
	}
	else
		wStatus1 &= ~WMSTAT_CYCLEOVERRUN;

	if (cycleEnd - stopTime > 0)
		delay((WM_LWORD)(cycleEnd - stopTime));
}

void VMArduino::VMP_PreProgram(void)
{
	// initialize random number generator
	srand( millis() );
	return;
}

void VMArduino::VMP_PreNextCommand(void)
{
	//Serial.println(wProgramCounter);
}

void VMArduino::VMP_ReadRTC(WM_DATE_AND_TIME *dt)
{

/*
	dt->date.date.year = ltime->tm_year + 1900;
	dt->date.date.day = ltime->tm_mday;
	dt->date.date.month = ltime->tm_mon+1;
	dt->tod.tod.hour = ltime->tm_hour;
	dt->tod.tod.min = ltime->tm_min;
	dt->tod.tod.sec = ltime->tm_sec;
	dt->tod.tod.csec = 0;
*/

	return;
}


WM_TIME VMArduino::VMP_CurrentTime()
{
	return (WM_TIME)millis();
}

WM_REAL VMArduino::VMP_GetRandom()
{
	int l = rand();
	WM_REAL r = ((WM_REAL)l) / RAND_MAX;
	return r;
}

int VMArduino::WM_GetData(VMVariable* var, WM_BYTE* buf)
{
	if (!var)
		return 0;

	return WM_GetData(var->GetAddress(), (WM_BYTE)var->GetSize(), buf);
}

WM_BYTE VMArduino::WM_GetDataByte(VMVariable* var)
{
	WM_BYTE result;

	if (!var)
		return 0;

	if (WM_GetData(var, &result) == var->GetSize())
		return result; 

	Serial.println("Cannot get BYTE value");	

	return 0xFF;
}

int VMArduino::WM_SetData(VMVariable* var, WM_BYTE* buf)
{
	if (!var)
		return 0;

	return WM_SetData(var->GetAddress(), (WM_BYTE)var->GetSize(), buf);
}

int VMArduino::WM_SetDataByte(VMVariable* var, WM_BYTE value)
{
	if (!var)
		return 0;

	return WM_SetData(var, &value); 
}
