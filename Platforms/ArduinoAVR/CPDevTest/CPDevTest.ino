#include "src/vm_arduino.h"

#ifndef USE_SD_XCP
#include "xcp/WeJeStSt.c"
#endif

VMArduino cpdev;

#define LEDPIN    LED_BUILTIN
#define BUTTONPIN 6

WM_BOOL onof = 0;
WM_BOOL o0, o1, o2, o3; 

// Note: For Due use Serial1 (pins 18 and 19) or Serial2 for messages (Serial does not work)
HardwareSerial& HWSerial = Serial1;

void setup()
{
  HWSerial.begin(9600);
  while(!HWSerial);

  HWSerial.println("CPDev test");

  cpdev.HWSerial = &HWSerial;

#ifdef USE_SD_XCP
  if (cpdev.VMP_LoadProgramAndData("xcp/WeJeStSt.xcp") != 0)    // note that SD library supports short (8+3) filenames only
#else
  if (cpdev.VMP_LoadProgramFromArray(WeJeStSt_xcp, DEFAULT_DATA_SIZE) != 0)
#endif  
  {
    HWSerial.print("Cannot load program into VM");
    while (1);
  }
  else
  {   
    HWSerial.println("VM initializing...");
    cpdev.task_cycle = 100;
    cpdev.WM_Initialize(WM_MODE_FIRST_START | WM_MODE_NORMAL);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(1, INPUT_PULLUP);
  }
}

void loop()
{
  if (cpdev.bRunMode)
  {
    onof = 1; //digitalRead(BUTTONPIN);  // set to 1 if you dont have a pushbutton
    cpdev.WM_SetData(4, sizeof(onof), &onof);
    
    cpdev.WM_RunCycle();
    
    cpdev.WM_GetData(0, sizeof(WM_BOOL), &o0);
    cpdev.WM_GetData(1, sizeof(WM_BOOL), &o1);
    cpdev.WM_GetData(2, sizeof(WM_BOOL), &o2);
    cpdev.WM_GetData(3, sizeof(WM_BOOL), &o3);

    digitalWrite(LEDPIN, o0);
    HWSerial.print("Output:");
    HWSerial.print(o0);
    HWSerial.print(" ");
    HWSerial.print(o1);
    HWSerial.print(" ");
    HWSerial.print(o2);
    HWSerial.print(" ");
    HWSerial.print(o3);
    HWSerial.println();
  }
  else
    cpdev.WM_Shutdown();

}
