#include "vm_linux.h"

#include <stdio.h>
#include <time.h>

#ifdef USE_SD_XCP

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>
#include <sys/resource.h>

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

unsigned long long int micros()
{
	/*
	struct rusage usage;
	struct timeval ts;

	getrusage(RUSAGE_SELF, &usage);

	ts = usage.ru_utime;
	printf("%ld", ts.tv_usec);
	return ts.tv_usec + (ts.tv_sec * 1000 * 1000);
	*/

	struct timespec ts;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);	
	return ts.tv_sec * 1000 * 1000 * 1000 + ts.tv_nsec;
}

int VMLinux::VMP_LoadProgramAndData(const char* file, int datasize)
{
#ifdef USE_SD_XCP
	long len;

	int res;
	struct stat buf;

	res = open(file, O_RDONLY);

	if (res == -1) return -1;

	len = fstat(res, &buf);
	if (len == -1) return -1;

	// allocate memory for the code
	pgmCode = (WM_BYTE*)malloc(buf.st_size);
	if (!pgmCode) return -3;

	// read the code into the allocated memory
	read(res, pgmCode, buf.st_size);

	close(res);

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
	cStartTime = micros();
	//printf("Start at %llu", cStartTime);
}

void VMLinux::VMP_PostCycle(void)
{
	unsigned long long int stopTime, cycleEnd;

	if (task_cycle == 0)
		return;

	stopTime = micros();
	//printf(" Stop at %llu", stopTime);

	cycleEnd = cStartTime + task_cycle * 1000; 

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
		usleep(cycleEnd - stopTime);
}

void VMLinux::VMP_PreProgram(void)
{
	// initialize random number generator
	srand( micros() );
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
	return (WM_TIME)(micros() / 1000);
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

