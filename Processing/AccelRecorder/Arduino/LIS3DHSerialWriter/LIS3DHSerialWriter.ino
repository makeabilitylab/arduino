//For scrolling:
// - https://forums.adafruit.com/viewtopic.php?f=25&t=112175

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// Used for LIS3DH hardware & software SPI
#define LIS3DH_CS 10
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

const boolean INCLUDE_TIMESTAMP = true;

void setup() {
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
  delay(10);
}
