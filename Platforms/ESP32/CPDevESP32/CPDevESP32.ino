
#include <vm_arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "WeJeStSt.c"

const int led1 = 23; // Pin of the LED
const int but1 = 22; // Pin of the button

// Replace the next variables with your SSID/Password combination
const char* ssid = "*******";
const char* password = "******";

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "192.168.1.70";

VMArduino cpdev;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(led1, OUTPUT);

  Serial.begin(112500);
  delay(1000);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  cpdev.VMP_LoadProgramFromArray((WM_BYTE*)Kc_BLINK);
    
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
  // put your main code here, to run repeatedly:
  if (!client.connected())
  {
    reconnect();
  }

  WM_BYTE out0;
  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
       
    char tempString[8];
    cpdev.WM_GetData(0, 1, &out0);
    client.publish("cpdev/out0", out0 ? "1" : "0");
    cpdev.WM_GetData(1, 1, &out0);
    client.publish("cpdev/out1", out0 ? "1" : "0");
    cpdev.WM_GetData(2, 1, &out0);
    client.publish("cpdev/out2", out0 ? "1" : "0");
    cpdev.WM_GetData(3, 1, &out0);
    client.publish("cpdev/out3", out0 ? "1" : "0");
   
  }    

  client.loop();
  
}

void cpdev_task(void * parameter)
{
  //cpdev.task_cycle = 100;
  cpdev.WM_Initialize(WM_MODE_FIRST_START | WM_MODE_NORMAL); 

  for(;;)
  {
    WM_BYTE light, button; 
    WM_INT count;

    //button = digitalRead(but1);
    //cpdev.WM_SetData(0,0,&button);
    cpdev.WM_RunCycle();
    cpdev.WM_GetData(1, 1, &light);
    digitalWrite(led1, light); 
    //Serial.println(light);
    
    //cpdev.WM_GetData(2, 2, (WM_BYTE*)&count);
    //Serial.println(count);

    // Pause the task again for 500ms
    //vTaskDelay(500 / portTICK_PERIOD_MS);
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

  WM_BYTE in0; 

  if (String(topic) == "cpdev/in0")
   {
    Serial.print("Changing in0 to ");
    if(messageTemp == "on"){
      Serial.println("on");
      in0 = 1;
      cpdev.WM_SetData(4,1,&in0);
    }
    else if(messageTemp == "off")
    {
      Serial.println("off");
      in0 = 0;
      cpdev.WM_SetData(4,1,&in0);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
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
