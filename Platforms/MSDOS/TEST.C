#include <stdio.h>

#include "vm_bc.h"

#ifndef USE_XCP_FILE
#include "xcp/WeJeStSt.c"
#endif

int main(int argc, char** argv) {
	
	int result;

	WM_BOOL onof = 0;
	WM_BOOL o0, o1, o2, o3;	

#ifdef USE_XCP_FILE
	char* xcp;

	if (argc > 1)
	{
		xcp = argv[1];
	}

	if (result = VMP_LoadProgramAndData(xcp, DEFAULT_DATA_SIZE))
	{
		printf("Cannot load %s : %d\n", xcp, result);
		return -1;
	}
#else
	if (result = VMP_LoadProgramFromArray(WeJeStSt_xcp, DEFAULT_DATA_SIZE))
	{
		printf("Cannot load : %d\n", result);
		return -1;
	}
#endif

	task_cycle = 200;
	WM_Initialize(WM_MODE_FIRST_START | WM_MODE_NORMAL);

	while(bRunMode)
	{
		// todo: read button state from a pin, key, etc.
		WM_SetData(4, 1, &onof);

		WM_RunCycle();

		WM_GetData(0, 1, &o0);
		WM_GetData(1, 1, &o1);
		WM_GetData(2, 1, &o2);
		WM_GetData(3, 1, &o3);
		printf("Output: %d %d %d %d\n", o0, o1, o2, o3);
	}

}

