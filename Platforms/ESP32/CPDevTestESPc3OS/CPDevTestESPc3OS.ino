#include <WiFi.h>
#include <PubSubClient.h>

#define USE_SDIFFS

#ifdef USE_SDIFFS
#include "SPIFFS.h"
#endif

#include "src/vm_arduino.h"
#include "src/vm_variable.h"

#ifndef USE_SD_XCP
#include "xcp/WeJeStSt.c"
#endif

#define LED1PIN   3 // LED_BUILTIN
#define LED2PIN   4 // LED_BUILTIN
#define LED3PIN   5 // LED_BUILTIN
#define BUTTONPIN 6

VMArduino cpdev;
VMDCP dcpInfo;
VMVariable *out0, *out1, *out2, *out3, *onof;

WM_BYTE output;
WM_BYTE button = 1;

const char* ssid = "Bosmanet";
const char* password = "anilorak";

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "192.168.31.15";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

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

  pinMode(BUTTONPIN, INPUT_PULLUP);

  pinMode(LED1PIN, OUTPUT);  
  pinMode(LED2PIN, OUTPUT);
  pinMode(LED3PIN, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  xTaskCreate(
    cpdev_task,    // Function that should be called
    "CPDev task",   // Name of the task (for debugging)
    1000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL             // Task handle
  );

}

void loop() {

  if (!client.connected())
  {
    reconnect();
  }

  WM_BYTE out;
  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
       
    char tempString[8];
    cpdev.WM_GetData(out0, &out);
    client.publish("cpdev/out0", out ? "1" : "0");
    cpdev.WM_GetData(out1, &out);
    client.publish("cpdev/out1", out ? "1" : "0");
    cpdev.WM_GetData(out2, &out);
    client.publish("cpdev/out2", out ? "1" : "0");
    cpdev.WM_GetData(out3, &out);
    client.publish("cpdev/out3", out ? "1" : "0");
   
    client.publish("cpdev/in0", button ? "on" : "off");
  }    

  client.loop();

}

void cpdev_task(void * parameter)
{
  
  Serial.println("VM initializing...");
  cpdev.task_cycle = 100;
  cpdev.WM_Initialize(WM_MODE_FIRST_START | WM_MODE_NORMAL);


  while (cpdev.bRunMode)
  {
    //button = digitalRead(BUTTONPIN);  // set to 1 if you dont have a pushbutton
    cpdev.WM_SetData(onof->GetAddress(),onof->GetSize(), &button);
    
    cpdev.WM_RunCycle();

    cpdev.WM_GetData(out0, &output);
    digitalWrite(LED1PIN, output); 
    cpdev.WM_GetData(out1, &output);
    digitalWrite(LED2PIN, output);
    cpdev.WM_GetData(out2, &output);
    digitalWrite(LED3PIN, output);
    cpdev.WM_GetData(out3, &output);

    if (output)
    {
      digitalWrite(LED1PIN, output); 
      digitalWrite(LED2PIN, output); 
      digitalWrite(LED3PIN, output);
    } 
    
    //button = 0; // simulate button depressed
  }
  
  cpdev.WM_Shutdown(); 
    
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
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message

  //WM_BYTE in0; 

  if (String(topic) == "cpdev/in0")
   {
    Serial.print("Changing in0 to ");
    if(messageTemp == "on"){
      Serial.println("on");
      button = 1;
      //cpdev.WM_SetData(onof,&in0);
    }
    else if(messageTemp == "off")
    {
      Serial.println("off");
      button = 0;
      //cpdev.WM_SetData(onof,&in0);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("cpdev/in0");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
