
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>


void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);  
  WiFiManager wifiManager;  
  wifiManager.autoConnect("Smart curtains");  
  Serial.println("Connected.....");  

}

void loop() {
  // put your main code here, to run repeatedly:

}
