/*
 * Prints MAC address of ESP32
 * See instructions here: 
 *   - https://github.com/jonfroehlich/CSE599Au2019/wiki/Setting-up-ESP32-Feather-board-for-campus-WiFi-connection
 * 
 * By Liang He and Jon Froehlich
 * http://makeabilitylab.io
 * 
 */
#include <WiFi.h>
 
void setup(){
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
}

void loop(){
  Serial.print("Device MAC address:");
  Serial.println(WiFi.macAddress());
  delay(2000);
}
