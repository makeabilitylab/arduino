/*
 * Reads in the x, y, and z accelerometer values from the LIS3DH and a "record gesture" button
 * state and prints the following CSV to serial: timestamp, x, y, z, buttonState
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// Used for LIS3DH hardware & software SPI
#define LIS3DH_CS 10
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

const boolean INCLUDE_TIMESTAMP = true; // print out timestamp to serial
const int BUTTON_INPUT_PIN = 21; // hooked up with pull-up configuration
const int SERIAL_BAUD_RATE = 115200; // make sure this matches the value in GestureRecorder.pde
const int DELAY_MS = 10; // the loop delay

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("Initializing accelerometer...");
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");

  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = ");
  Serial.print(2 << lis.getRange());
  Serial.println("G");

  pinMode(BUTTON_INPUT_PIN, INPUT_PULLUP);
}

void loop() {

  // Read accel data
  lis.read();      

  int buttonVal = digitalRead(BUTTON_INPUT_PIN);

  if(INCLUDE_TIMESTAMP){
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

  if(DELAY_MS > 0){
    delay(DELAY_MS);
  }
}
