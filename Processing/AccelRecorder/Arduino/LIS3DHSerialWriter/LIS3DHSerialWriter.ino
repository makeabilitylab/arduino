/*
 * Reads in the x, y, and z accelerometer values from the LIS3DH and prints the following CSV
 * to serial: timestamp, x, y, z.
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

const boolean INCLUDE_TIMESTAMP = true;

void setup() {

  // To increase speed, can change this to Serial.begin(115200);
  Serial.begin(9600);
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
}

void loop() {
  // put your main code here, to run repeatedly:
  lis.read();      // get X Y and Z data at once
   // Then print out the raw data

  if(INCLUDE_TIMESTAMP){
    Serial.print(millis());
    Serial.print(", ");
  }
   
  Serial.print(lis.x);
  Serial.print(", ");
  Serial.print(lis.y);
  Serial.print(", ");
  Serial.print(lis.z);
  Serial.println();

  // the delay here will obviously affect the sampling rate of our accel sensor
  delay(10);  
}
