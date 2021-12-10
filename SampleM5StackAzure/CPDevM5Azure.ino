#include <M5Stack.h>
#include <WiFi.h>
#include "Esp32MQTTClient.h"

#include <vm_arduino.h>
#include <vm_variable.h>

#include "WeJeStSt.c"

#define USE_SD_XCP

const int led1 = 21; // Pin of the LED
const int but1 = 22; // Pin of the button

// Replace the next variables with your SSID/Password combination
const char* ssid = "*******";
const char* password = "*******";

#define INTERVAL 10000
#define MESSAGE_MAX_LEN 256

/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
static const char* connectionString = "HostName=******;DeviceId=******;SharedAccessKey=******";
const char *messageData = "{\"messageId\":%d, \"out0\":%d, \"out1\":%d, \"out2\":%d, \"out3\":%d}";
static bool hasIoTHub = false;
static bool hasWifi = false;
int messageCount = 1;
static bool messageSending = true;
static uint64_t send_interval_ms;

VMArduino cpdev;
VMDCP dcpInfo;

VMVariable *out0, *out1, *out2, *out3, *onof;

WM_BYTE in0; 

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
{
  if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
  {
    Serial.println("Send Confirmation Callback finished.");
  }
}

static void MessageCallback(const char* payLoad, int size)
{
  Serial.println("Message callback:");
  Serial.println(payLoad);
}

static void DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size)
{
  char *temp = (char *)malloc(size + 1);
  if (temp == NULL)
  {
    return;
  }
  memcpy(temp, payLoad, size);
  temp[size] = '\0';
  // Display Twin message.
  Serial.println(temp);
  free(temp);
}

static int DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
{
  LogInfo("Try to invoke method %s", methodName);
  const char *responseMessage = "\"Successfully invoke device method\"";
  int result = 200;

  if (strcmp(methodName, "start") == 0)
  {
    LogInfo("Start sending temperature and humidity data");
    messageSending = true;
  }
  else if (strcmp(methodName, "stop") == 0)
  {
    LogInfo("Stop sending temperature and humidity data");
    messageSending = false;
  }
  else if (strcmp(methodName, "on") == 0)
  {
    LogInfo("Set input to on");
    in0 = 1;
  }
  else if (strcmp(methodName, "off") == 0)
  {
    LogInfo("Set input to off");
    in0 = 0;
  }
  else
  {
    LogInfo("No method %s found", methodName);
    responseMessage = "\"No method found\"";
    result = 404;
  }

  *response_size = strlen(responseMessage) + 1;
  *response = (unsigned char *)strdup(responseMessage);

  return result;
}

void setup() {

  M5.begin();
  M5.Lcd.setBrightness(255);
  
  // put your setup code here, to run once:
  pinMode(led1, OUTPUT);

  Serial.begin(112500);
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

  
  setup_wifi();

}

void loop()
{
  if (hasWifi && hasIoTHub)
  {
    if (messageSending && (int)(millis() - send_interval_ms) >= INTERVAL)
    {
        WM_BYTE o0, o1, o2, o3;
        o0 = cpdev.WM_GetDataByte(out0);
        o1 = cpdev.WM_GetDataByte(out1);
        o2 = cpdev.WM_GetDataByte(out2);
        o3 = cpdev.WM_GetDataByte(out3);
    
      // Send teperature data
      char messagePayload[MESSAGE_MAX_LEN];
      snprintf(messagePayload, MESSAGE_MAX_LEN, messageData, messageCount++, o0, o1, o2, o3);
      Serial.println(messagePayload);
      EVENT_INSTANCE* message = Esp32MQTTClient_Event_Generate(messagePayload, MESSAGE);
      Esp32MQTTClient_SendEventInstance(message);
      send_interval_ms = millis();
    }
    else
    {
      Esp32MQTTClient_Check();
    }
  }
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


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    hasWifi = false;
  }
  hasWifi = true;

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(" > IoT Hub");
  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString, true))
  {
    hasIoTHub = false;
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  hasIoTHub = true;
  Esp32MQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
  Esp32MQTTClient_SetMessageCallback(MessageCallback);
  Esp32MQTTClient_SetDeviceTwinCallback(DeviceTwinCallback);
  Esp32MQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);
  Serial.println("Start sending events.");
  randomSeed(analogRead(0));
  send_interval_ms = millis();
}
