#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <RCSwitch.h>
#include <PubSubClient.h>
#include <functional>
#include "Switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
#define HOSTNAME "RFSockets-OTA-" 
// prototypes
boolean connectWifi();
//on/off callbacks 
void AOn();
void AOff();
void BOn();
void BOff();
void COn();
void COff();
void DOn();
void DOff();
// Change this before you flash
const char* ssid = "TNCAPA11CF7";
const char* password = "004B7E3FE8";
const char* mqtt_server = "192.168.1.2";
WiFiClient espClient;
PubSubClient client(espClient);
boolean wifiConnected = false;
RCSwitch mySwitch = RCSwitch();
UpnpBroadcastResponder upnpBroadcastResponder;
Switch *A = NULL;
Switch *B = NULL;
Switch *C = NULL;
Switch *D = NULL;
void setup()
{
  Serial.begin(9600);
  ArduinoOTA.onError([](ota_error_t error) { ESP.restart(); });
  ArduinoOTA.begin();
  mySwitch.enableTransmit(0);
  mySwitch.setPulseLength(180);
  // Initialise wifi connection
  wifiConnected = connectWifi();
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    // Define your switches here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    A = new Switch("table light", 81, AOn, AOff);
    B = new Switch("light", 82, BOn, BOff);
    C = new Switch("fire", 83, COn, COff);
    D = new Switch("everything", 84, DOn, DOff);
    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*A);
    upnpBroadcastResponder.addDevice(*B);
    upnpBroadcastResponder.addDevice(*C);
    upnpBroadcastResponder.addDevice(*D);
  }
}
 
void loop()
{
   if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      A->serverLoop();
      B->serverLoop();
      C->serverLoop();
      D->serverLoop();
      ArduinoOTA.handle();
   } else {
    wifiConnected = connectWifi();
   }
}

void AOn() {
    Serial.print("Switch 1 turn on ...");
    mySwitch.send("010000010101010100110011");
}

void AOff() {
    Serial.print("Switch 1 turn off ...");
    mySwitch.send("010000010101010100111100");
}
void BOn() {
    Serial.print("Switch 2 turn on ...");
    mySwitch.send("010000010101010111000011");
}

void BOff() {
    Serial.print("Switch 2 turn off ...");
    mySwitch.send("010000010101010111001100");
}
void COn() {
    Serial.print("Switch 3 turn on ...");
    mySwitch.send("010000010101011100000011");
}

void COff() {
    Serial.print("Switch 3 turn off ...");
    mySwitch.send("010000010101011100001100");
}
void DOn() {
    Serial.print("All turn on ...");
    mySwitch.send("010000010111010100000011");
}

void DOff() {
    Serial.print("All turn off ...");
    mySwitch.send("010000010111010100001100");
}
// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}
