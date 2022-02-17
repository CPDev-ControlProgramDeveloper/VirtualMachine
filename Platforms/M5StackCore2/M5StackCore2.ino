#include <M5Core2.h>

// enable to load the XCP from SD card
//#define USE_SD_XCP

#include "src/vm_arduino.h"
#include "src/vm_variable.h"

#ifndef USE_SD_XCP
#include "WeJeStSt.c"
#endif


const int led1 = 21; // Pin of the LED
const int but1 = 22; // Pin of the button

VMArduino cpdev;
VMDCP dcpInfo;

VMVariable *out0, *out1, *out2, *out3, *onof;

WM_BYTE in0; 

void setup() {

  M5.begin();
  M5.Lcd.setBrightness(255);
  
  // put your setup code here, to run once:
  pinMode(led1, OUTPUT);

  Serial.begin(115200);
  delay(1000);

  int result;
#ifdef USE_SD_XCP
  if (result = cpdev.VMP_LoadProgramAndData("/WeJeStSt.xcp"))    // note that SD library supports short (8+3) filenames only
#else
  if (result = cpdev.VMP_LoadProgramFromArray((WM_BYTE*)Kc_BLINK))
#endif  
  {
    Serial.print("Cannot load program into VM: ");
    Serial.println(result);
    while (1);
  } 


#ifdef USE_SD_XCP
  if (!dcpInfo.Load("/WeJeStSt.dcp"))
  {
    Serial.println("DCP ok");
    out0 = dcpInfo.InitVariable("OUT0"); 
    out1 = dcpInfo.InitVariable("OUT1");
    out2 = dcpInfo.InitVariable("OUT2");
    out3 = dcpInfo.InitVariable("OUT3");
    onof = dcpInfo.InitVariable("ONOF");
  }
  else
  {
    Serial.println("Cannot load DCP ");
    while (1);
  }
#else
    out0 = new VMVariable("OUT0", 0, 1); 
    out1 = new VMVariable("OUT1", 1, 1);
    out2 = new VMVariable("OUT2", 2, 1);
    out3 = new VMVariable("OUT3", 3, 1);
    onof = new VMVariable("ONOF", 4, 1);  
#endif 
    
  xTaskCreate(
    cpdev_task,    // Function that should be called
    "CPDev task",   // Name of the task (for debugging)
    1000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL             // Task handle
  );

 
}

void loop()
{
 
  delay(10);  
}

void cpdev_task(void * parameter)
{
  //cpdev.task_cycle = 100;
  cpdev.WM_Initialize(WM_MODE_FIRST_START | WM_MODE_NORMAL); 

  for(;;)
  {
    WM_BYTE output;
    WM_BYTE button;

    button = M5.BtnA.isPressed() | in0;
    cpdev.WM_SetData(onof->GetAddress(),onof->GetSize(),&button);
        
    cpdev.WM_RunCycle();
    
    cpdev.WM_GetData(out0, &output);
    digitalWrite(led1, output); 
    M5.Lcd.fillCircle(30,100,30,output ? 0x51d : 0x7bef);
    
    cpdev.WM_GetData(out1, &output);
    M5.Lcd.fillCircle(100,100,30,output ? 0x51d : 0x7bef);
    
    cpdev.WM_GetData(out2, &output);
    M5.Lcd.fillCircle(170,100,30,output ? 0x51d : 0x7bef);
    
    cpdev.WM_GetData(out3, &output);
    M5.Lcd.fillCircle(240,100,30,output ? 0x51d : 0x7bef);
  
    M5.update();
  }

  /* delete a task when finish,
    this will never happen because this is infinity loop */
  vTaskDelete( NULL );
}
