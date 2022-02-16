#include "vm_windows.h"

#include <stdio.h>
#include <time.h>

#include <Windows.h>

#ifdef USE_SD_XCP

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
//#include <unistd.h>
#include <cmath>
//#include <sys/resource.h>

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

unsigned long long GetSystemClock()
{
	static BOOL initialized = FALSE;
	static LARGE_INTEGER frequency;
	static BOOL hires_timer_present = FALSE;

	if (!initialized)
	{
		hires_timer_present = QueryPerformanceFrequency(&frequency);
		initialized = TRUE;
	}

	if (hires_timer_present) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}


unsigned long long int millis()
{	
	return GetSystemClock();
}

int VMLinux::VMP_LoadProgramAndData(const char* file, int datasize)
{
#ifdef USE_SD_XCP
	long len;

	int res;
	struct stat buf;

	res = _open(file, O_RDONLY);

	if (res == -1) return -1;

	len = fstat(res, &buf);
	if (len == -1) return -1;

	// allocate memory for the code
	pgmCode = (WM_BYTE*)malloc(buf.st_size);
	if (!pgmCode) return -3;

	// read the code into the allocated memory
	_read(res, pgmCode, buf.st_size);

	_close(res);

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

int VMLinux::VMP_LoadProgramFromArray(const unsigned char* code, int datasize)
{
	if (datasize > sizeof(pgmDataDefault))
		return -4;

  	pgmCode = (WM_BYTE*) code;
  
	pgmData = pgmDataDefault;

	// set initial status of the machine
	wStatus1 = WMSTAT_COLDRESTART;

	// clear cycle counter
	nCycles = 0;

	return 0;	// success
}

void VMLinux::VMP_Debug(WM_BYTE err, WM_WORD aux)
{
	printf("Runtime error %d, %x near %x\n", err, aux, wProgramCounter);
	bRunMode = 0;
}

void VMLinux::VMP_PreCycle(void)
{
	cStartTime = millis();
	//printf("Start at %llu", cStartTime);
}

void VMLinux::VMP_PostCycle(void)
{
	unsigned long long int stopTime, cycleEnd;

	if (task_cycle == 0)
		return;

	stopTime = millis();
	//printf(" Stop at %llu", stopTime);

	cycleEnd = cStartTime + task_cycle; 

	if (stopTime > cycleEnd)
	{
		wStatus1 |= WMSTAT_CYCLEOVERRUN;
		printf("Cycle overrun !\n");
		return;
	}
	else
		wStatus1 &= ~WMSTAT_CYCLEOVERRUN;

	calcTime = stopTime - cStartTime;

	if (cycleEnd - stopTime > 0)
		Sleep((cycleEnd - stopTime)/1000);
}

void VMLinux::VMP_PreProgram(void)
{
	// initialize random number generator
	srand( millis() );
	return;
}

void VMLinux::VMP_PreNextCommand(void)
{	
	//printf("%d\n",wProgramCounter);
}

void VMLinux::VMP_ReadRTC(WM_DATE_AND_TIME *dt)
{

	time_t t;
	struct tm* ltime;
	time(&t);
	ltime = localtime(&t);

	dt->date.date.year = ltime->tm_year + 1900;
	dt->date.date.day = ltime->tm_mday;
	dt->date.date.month = ltime->tm_mon+1;
	dt->tod.tod.hour = ltime->tm_hour;
	dt->tod.tod.min = ltime->tm_min;
	dt->tod.tod.sec = ltime->tm_sec;
	dt->tod.tod.csec = 0;

	return;
}


WM_TIME VMLinux::VMP_CurrentTime()
{
	return (WM_TIME)(millis());
}

WM_REAL VMLinux::VMP_GetRandom()
{
	int l = rand();
	WM_REAL r = ((WM_REAL)l) / RAND_MAX;
	return r;
}

int VMLinux::WM_GetData(VMVariable* var, WM_BYTE* buf)
{
	if (!var)
		return 0;

	return WM_GetData(var->GetAddress(), (WM_BYTE)var->GetSize(), buf);
}

WM_BYTE VMLinux::WM_GetDataByte(VMVariable* var)
{
	WM_BYTE result;

	if (!var)
		return 0;

	if (WM_GetData(var, &result) == var->GetSize())
		return result; 

	throw;
}

int VMLinux::WM_SetData(VMVariable* var, WM_BYTE* buf)
{
	if (!var)
		return 0;

	return WM_SetData(var->GetAddress(), (WM_BYTE)var->GetSize(), buf);
}

int VMLinux::WM_SetDataByte(VMVariable* var, WM_BYTE value)
{
	if (!var)
		return 0;

	return WM_SetData(var, &value); 
}

