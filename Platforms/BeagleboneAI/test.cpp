#include <stdio.h>
#include <unistd.h>

#include <sys/resource.h>

#include "vm_linux.h"
#include "vm_variable.h"

VMLinux cpdev;
VMDCP dcpInfo;

VMVariable *out0, *out1, *out2, *out3, *onof;

FILE *brightness0, *brightness1, *brightness2, *brightness3;

unsigned long long int micros();

int main(int argc, char** argv) {
	
	int result;

	char* xcp = (char*)"xcp/WeJeStSt.xcp";

	if (argc > 1)
	{
		xcp = argv[1];
	}

	if (result = cpdev.VMP_LoadProgramAndData(xcp))
	{
		printf("Cannot load program %s into VM: %d\n", xcp, result);
		return -1;
	}

	char* dcp = (char*)malloc(strlen(xcp)+1);
	strcpy(dcp, xcp);

	char *ext = dcp + strlen(dcp);

    while (*ext != '.' && ext > dcp)
	{
        --ext;
    }

	if (strcmp(ext,".xcp"))
	{
		printf("Bad xcp filename: %s\n", dcp);
		return -2;
	}
	strncpy(ext,".dcp",3);

	if (!dcpInfo.Load(dcp))
	{
		printf("\n%s loaded\n", dcp);
		out0 = dcpInfo.InitVariable("OUT0");
		out1 = dcpInfo.InitVariable("OUT1");
		out2 = dcpInfo.InitVariable("OUT2");
		out3 = dcpInfo.InitVariable("OUT3");
		onof = dcpInfo.InitVariable("ONOF");
	}
	else
	{
		printf("Cannot load DCP !\n");
		return -2;
	}

#if 1
		FILE* trigger0 = fopen("/sys/class/leds/beaglebone:green:usr0/trigger", "w");
		FILE* trigger1 = fopen("/sys/class/leds/beaglebone:green:usr1/trigger", "w");
		FILE* trigger2 = fopen("/sys/class/leds/beaglebone:green:usr2/trigger", "w");
		FILE* trigger3 = fopen("/sys/class/leds/beaglebone:green:usr3/trigger", "w");

		brightness0 = fopen("/sys/class/leds/beaglebone:green:usr0/brightness", "w");
		brightness1 = fopen("/sys/class/leds/beaglebone:green:usr1/brightness", "w");
		brightness2 = fopen("/sys/class/leds/beaglebone:green:usr2/brightness", "w");
		brightness3 = fopen("/sys/class/leds/beaglebone:green:usr3/brightness", "w");

		if (trigger0)
			fprintf(trigger0, "none\n");
		if (trigger1)
			fprintf(trigger1, "none\n");
		if (trigger2)
			fprintf(trigger2, "none\n");
		if (trigger3)
			fprintf(trigger3, "none\n");
#endif

	cpdev.task_cycle = 200;
	cpdev.WM_Initialize(WM_MODE_FIRST_START | WM_MODE_NORMAL);

	WM_BOOL button = 0;

	WM_BYTE o0, o1, o2, o3;

	while(1)
	{
		// todo: read button state from a pin, key, etc.
		//cpdev.WM_SetData(onof, &button);

		cpdev.WM_RunCycle();

		cpdev.WM_GetData(out0, &o0);
		cpdev.WM_GetData(out1, &o1);
		cpdev.WM_GetData(out2, &o2);
		cpdev.WM_GetData(out3, &o3);
		printf("Output: %d %d %d %d\n", o0, o1, o2, o3);

#if 1
		if (brightness0)
			fwrite(o0 ? "1" : "0", sizeof(char), 1, brightness0);		
		if (brightness1)
			fwrite(o1 ? "1" : "0", sizeof(char), 1, brightness1);		
		if (brightness2)
			fwrite(o2 ? "1" : "0", sizeof(char), 1, brightness2);		
		if (brightness3)
			fwrite(o3 ? "1" : "0", sizeof(char), 1, brightness3);		
#endif


	}


	free(dcp);
}

