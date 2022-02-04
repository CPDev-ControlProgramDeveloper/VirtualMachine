
#include "src/vm_arduino.h"
#include "src/vm_variable.h"

#ifndef USE_XCP_FILE
#include "xcp/WeJeStSt.c"
#endif


VMArduino cpdev;

#define LEDPIN    13 //LED_BUILTIN
#define BUTTONPIN 6

WM_BOOL onof = 0;
WM_BOOL o0, o1, o2, o3; 

void setup()
{
  Serial.begin(9600);
  while(!Serial);

#ifdef USE_XCP_FILE
  if (cpdev.VMP_LoadProgramAndData("WeJeStSt.xcp") != 0)    // note that SD library supports short (8+3) filenames only
#else
  if (cpdev.VMP_LoadProgramFromArray(WeJeStSt_xcp, DEFAULT_DATA_SIZE) != 0)
#endif  
  {
    Serial.print("Cannot load program into VM");
    while (1);
  }
  else
  {   
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
    Serial.print("Output:");
    Serial.print(o0);
    Serial.print(" ");
    Serial.print(o1);
    Serial.print(" ");
    Serial.print(o2);
    Serial.print(" ");
    Serial.print(o3);
    Serial.println();
  }
  else
    cpdev.WM_Shutdown();

}
