#include "vm_6502.h"

#include <stdio.h>
#include <time.h>

#include <string.h>
#include <stdlib.h>

WM_BYTE pgmDataDefault[DEFAULT_DATA_SIZE];

WM_TIME	cStartTime;			// cycle starting time (for constant cycle calculation)

WM_TIME micros()
{
	return clock() * 20;
}

int VMP_LoadProgramAndData(const char* file, int datasize)
{
#ifdef USE_XCP_FILE
	long len;
	FILE* xcp;

	xcp = fopen(file, "r");

	if (!xcp) return -1;

	fseek(xcp, 0, SEEK_END);
	len = ftell(xcp);
	fseek(xcp, 0, SEEK_SET);

	// allocate memory for the code
	pgmCode = (WM_BYTE*)malloc(len);
	if (!pgmCode) return -3;

	// read the code into the allocated memory
	fread(pgmCode, len, 1, xcp);

	fclose(xcp);

	if (datasize > sizeof(pgmDataDefault))
	{
		// first release data memory if it has been previously allocated
		if (pgmData) free(pgmData);

		// if data size parameter is 0, use default data memory size
		if (!datasize) datasize = DEFAULT_DATA_SIZE;

		// allocate data memory and clear it
		pgmData = (WM_BYTE*)malloc(datasize);
		if (!pgmData) return -4;
	}
	else
		pgmData = pgmDataDefault;
	
	memset(pgmData, 0, datasize);

	// set initial status of the machine
	wStatus1 = WMSTAT_COLDRESTART;

	// clear cycle counter
	nCycles = 0;

#endif

	return 0;	// success
}

#ifndef USE_XCP_FILE

int VMP_LoadProgramFromArray(const unsigned char* code, int datasize)
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

#endif

void VMP_Debug(WM_BYTE err, WM_WORD aux)
{
	printf("Runtime error %d, %x near %x\n", err, aux, wProgramCounter);
	bRunMode = 0;
}

void VMP_PreCycle(void)
{
	cStartTime = micros();
	//printf("Start at %llu", cStartTime);
}

void VMP_PostCycle(void)
{
	WM_TIME stopTime, cycleEnd;

	if (task_cycle == 0)
		return;

	stopTime = micros();
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

	while (micros() < stopTime)
		;
}

void VMP_PreProgram(void)
{
#ifdef VM_REAL_SUPPORT	
	// initialize random number generator
	srand( micros() );
#endif	
	return;
}

void VMP_PreNextCommand(void)
{	
	//printf("%d\n",wProgramCounter);
}

#ifdef VM_DATETIME_SUPPORT
void VMP_ReadRTC(WM_DATE_AND_TIME *dt)
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
#endif

WM_TIME VMP_CurrentTime()
{
	return (WM_TIME)(micros());
}

#ifdef VM_REAL_SUPPORT
WM_REAL VMP_GetRandom()
{
	int l = rand();
	WM_REAL r = ((WM_REAL)l) / RAND_MAX;
	return r;
}
#endif

 void VMP_PostProgram(void) {};
