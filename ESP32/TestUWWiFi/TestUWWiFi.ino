/*
 * Tests ESP32 WiFi connection on UW campus
 * 
 * See instructions here: 
 *   - https://github.com/jonfroehlich/CSE599Au2019/wiki/Setting-up-ESP32-Feather-board-for-campus-WiFi-connection
 * 
 * 
 * By Liang He
 * http://makeabilitylab.io
 * 
 */
 
#include <WiFi.h>

// WiFi related
const char* ssid = "University of Washington";
const char* password =  "";

void setup(){

  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // the network is successfully connected
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop(){
  // Intentionally blank  
}
