/*
   Reads in the x, y, and z accelerometer values from the LIS3DH and a "record gesture" button
   state and prints the following CSV to serial: timestamp, x, y, z, buttonState

   By Jon E. Froehlich
   @jonfroehlich
   http://makeabilitylab.io

*/

#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include "BluetoothSerial.h" //Header File for Serial Bluetooth, will be added by default into Arduino

// Settings
enum CommunicationMode {
  RECORDER_SERIAL,
  RECORDER_BLUETOOTH,
  RECORDER_WIFI
};
// By default we use serial
const CommunicationMode CURRENT_MODE = RECORDER_SERIAL; // CHANGE THIS IF YOU WANT A DIFFERENT MODE

// -------------------------------------------
// Serial info
const int SERIAL_BAUD_RATE = 115200;

// -------------------------------------------
// Bluetooth settings
const char* ARDUINO_BT_NAME = "ESP32 Gesture Recorder";
// Bluetooth uses the serial baud rate and port index above

// -------------------------------------------
// WIFI
// If using wifi, make sure to put your credentials there if you're using WIFI mode
const char* WIFI_SSID = "";
const char* WIFI_PASS = "";
const char* WIFI_HOST_IP_ADDRESS = ""; // The IP address of the host you want to connect to
const int WIFI_HOST_PORT = 10002; // The IP address of the host you want to connect to

// Used for LIS3DH hardware & software SPI
#define LIS3DH_CS 10
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

const boolean INCLUDE_TIMESTAMP = true; // print out timestamp to serial
const int BUTTON_INPUT_PIN = 21;        // hooked up with pull-up configuration
const int DELAY_MS = 10;                // the loop delay

// Common data objects
BluetoothSerial ESP_BT; //Object for Bluetooth
// Use WiFiClient class to create TCP connections
WiFiClient ESP_WIFI;

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("Initializing accelerometer...");
  if (!lis.begin(0x18))
  { // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1)
      yield();
  }
  Serial.println("LIS3DH found!");

  lis.setRange(LIS3DH_RANGE_4_G); // 2, 4, 8 or 16 G!

  Serial.print("Range = ");
  Serial.print(2 << lis.getRange());
  Serial.println("G");

  // Setup Bluetooth if we want
  if (CURRENT_MODE == RECORDER_BLUETOOTH) {
    if (!ESP_BT.begin(ARDUINO_BT_NAME)) {
      Serial.println("We failed to start Bluetooth");
    }
    ESP_BT.setPin("1234");
    Serial.println("Bluetooth started");
    while (!ESP_BT.isReady()) {
      Serial.println("Waiting for Bluetooth...");
      vTaskDelay(1000); // this is similar to yield and allows the device to go to low power mode
    }
  }
  else if (CURRENT_MODE == RECORDER_WIFI) {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting.");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      vTaskDelay(500); // this is similar to yield and allows the device to go to low power mode
    }
    Serial.print("WiFi connected - IP address: ");
    Serial.println(WiFi.localIP());
    if (!ESP_WIFI.connect(WIFI_HOST_IP_ADDRESS, WIFI_HOST_PORT)) {
      Serial.print("Failed to connect to ");
      Serial.print(WIFI_HOST_IP_ADDRESS);
      Serial.print(":");
      Serial.print(WIFI_HOST_PORT);
      Serial.println();
      while(1) vTaskDelay(5000);
    }
    Serial.println("Connected to host");
    ESP_WIFI.println("Hello");
  }

  pinMode(BUTTON_INPUT_PIN, INPUT_PULLUP);
}

void serial_output()
{
  int buttonVal = digitalRead(BUTTON_INPUT_PIN);

  if (INCLUDE_TIMESTAMP)
  {
    Serial.print(millis());
    Serial.print(", ");
  }

  Serial.print(lis.x);
  Serial.print(", ");
  Serial.print(lis.y);
  Serial.print(", ");
  Serial.print(lis.z);
  Serial.print(", ");
  Serial.print(!buttonVal); // because pull-up
  Serial.println();
}

void bluetooth_output()
{
  int buttonVal = digitalRead(BUTTON_INPUT_PIN);

  if (INCLUDE_TIMESTAMP)
  {
    ESP_BT.print(millis());
    ESP_BT.print(", ");
  }
  ESP_BT.print(lis.x);
  ESP_BT.print(", ");
  ESP_BT.print(lis.y);
  ESP_BT.print(", ");
  ESP_BT.print(lis.z);
  ESP_BT.print(", ");
  ESP_BT.print(!buttonVal); // because pull-up
  ESP_BT.println();
}

void wifi_output()
{
  int buttonVal = digitalRead(BUTTON_INPUT_PIN);

  if (INCLUDE_TIMESTAMP)
  {
    ESP_WIFI.print(millis());
    ESP_WIFI.print(", ");
  }
  ESP_WIFI.print(lis.x);
  ESP_WIFI.print(", ");
  ESP_WIFI.print(lis.y);
  ESP_WIFI.print(", ");
  ESP_WIFI.print(lis.z);
  ESP_WIFI.print(", ");
  ESP_WIFI.print(!buttonVal); // because pull-up
  ESP_WIFI.println();
}

void loop()
{

  // Read accel data
  lis.read();

  // Then output our stuff
  switch (CURRENT_MODE)
  {
    case RECORDER_SERIAL:
      serial_output();
      break;
    case RECORDER_BLUETOOTH:
      bluetooth_output();
      break;
    case RECORDER_WIFI:
      wifi_output();
      break;
  }

  if (DELAY_MS > 0)
  {
    delay(DELAY_MS);
  }
}
