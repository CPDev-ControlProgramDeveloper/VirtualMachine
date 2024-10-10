
#define USE_SDIFFS

#ifdef USE_SDIFFS
#include "SPIFFS.h"
#endif

#include "src/vm_arduino.h"
#include "src/vm_variable.h"

#ifndef USE_SD_XCP
#include "xcp/WeJeStSt.c"
#endif

VMArduino cpdev;

#define LED1PIN   3 // LED_BUILTIN
#define LED2PIN   4 // LED_BUILTIN
#define LED3PIN   5 // LED_BUILTIN
#define BUTTONPIN 6

VMDCP dcpInfo;
VMVariable *out0, *out1, *out2, *out3, *onof;

WM_BYTE output;
WM_BYTE button = 1;

void setup() {

  Serial.begin(9600);
  while(!Serial);

  Serial.println("CPDev test");

#ifdef USE_SD_XCP
  if (cpdev.VMP_LoadProgramAndData("xcp/WeJeStSt.xcp") != 0)    // note that SD library supports short (8+3) filenames only
#else

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File file = SPIFFS.open("/WeJeStSt.xcp");
  if(!file){
    Serial.println("Failed to open XCP for reading");
    return;
  }

  static uint8_t buf[512];
  size_t len = file.size();

  if (len > 512)
  {
    Serial.print("XCP is too large !");
    while (1);
  }
  
  file.read(buf, file.size());
  file.close();
  
  if (cpdev.VMP_LoadProgramFromArray(buf, DEFAULT_DATA_SIZE) != 0)
 
#endif  
  {
    Serial.print("Cannot load program into VM");
    while (1);
  }

#ifdef USE_SDIFFS

  File fileDCP = SPIFFS.open("/WeJeStSt.dcp");
  if(!fileDCP){
    Serial.println("Failed to open DCP for reading");
    return;
  }
  
  if (!dcpInfo.Load(fileDCP))
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

  
  Serial.println("VM initializing...");
  cpdev.task_cycle = 100;
  cpdev.WM_Initialize(WM_MODE_FIRST_START | WM_MODE_NORMAL);

  pinMode(BUTTONPIN, INPUT_PULLUP);

  pinMode(LED1PIN, OUTPUT);  
  pinMode(LED2PIN, OUTPUT);
  pinMode(LED3PIN, OUTPUT);
}

void loop() {
 
  if (cpdev.bRunMode)
  {
    button = digitalRead(BUTTONPIN);  // set to 1 if you dont have a pushbutton
    cpdev.WM_SetData(onof, &button);
    
    cpdev.WM_RunCycle();

    cpdev.WM_GetData(out0, &output);
    digitalWrite(LED1PIN, output); 
    cpdev.WM_GetData(out1, &output);
    digitalWrite(LED2PIN, output);
    cpdev.WM_GetData(out2, &output);
    digitalWrite(LED3PIN, output);
    cpdev.WM_GetData(out3, &output);

    button = 0; // simulate button depressed
  }
  else
    cpdev.WM_Shutdown();  
  
}
