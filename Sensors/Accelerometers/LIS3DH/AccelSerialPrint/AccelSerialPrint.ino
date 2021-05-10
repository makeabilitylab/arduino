
/**
 *  Read raw LIS3DH values and convert to Gs. Also, compare to Adafruit Sensor library.
 *  
 *  Adafruit LIS3DH tutorial:
 *  https://learn.adafruit.com/adafruit-lis3dh-triple-axis-accelerometer-breakout
 *
 *  By Jon E. Froehlich
 *  @jonfroehlich
 *  http://makeabilitylab.io
 *
 */

#include <Wire.h>
#include <SPI.h>

// For accelerometer
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// Used for LIS3DH hardware & software SPI
#define LIS3DH_CS 10
Adafruit_LIS3DH _lis3dh = Adafruit_LIS3DH();

// Copied these defines from https://github.com/adafruit/Adafruit_Sensor/blob/master/Adafruit_Sensor.h
//#define SENSORS_GRAVITY_EARTH (9.80665F) /**< Earth's gravity in m/s^2 */
//#define SENSORS_GRAVITY_MOON (1.6F)      /**< The moon's gravity in m/s^2 */
//#define SENSORS_GRAVITY_SUN (275.0F)     /**< The sun's gravity in m/s^2 */
//#define SENSORS_GRAVITY_STANDARD (SENSORS_GRAVITY_EARTH)

const int DELAY_LOOP_MS = 100; // change to slow down how often to read and graph value
const int MIN_ACCEL_VAL = -32768;
const int MAX_ACCEL_VAL = 32767;

void setup() {
  Serial.begin(9600);

  if (!_lis3dh.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldn't start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");

  _lis3dh.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = ");
  Serial.print(2 << _lis3dh.getRange());
  Serial.println("G");
}

void loop() {

  // For accelerometer range, see:
  // https://learn.adafruit.com/adafruit-lis3dh-triple-axis-accelerometer-breakout/arduino#accelerometer-ranges-1729247-23
  Serial.print("Range: ");
  Serial.print(2 << _lis3dh.getRange());
  Serial.println("G");

  // Read the accel
  _lis3dh.read(); 

  // Then print out the raw data
  Serial.println((String)"RAW\t\tX: " + _lis3dh.x + " Y: " + _lis3dh.y + " Z: " + _lis3dh.z);

  float x, y, z;
  getNormalizeReadings(_lis3dh, x, y, z);
  Serial.println((String)"RAW NORMALIZED\tX: " + x + " Y: " + y + " Z: " + z + " m/s^2");

  // Use Adafruit Sensor Library to get a normalized reading
  // https://learn.adafruit.com/using-the-adafruit-unified-sensor-driver/how-does-it-work
  sensors_event_t event;
  _lis3dh.getEvent(&event);

  // Display the results (acceleration is measured in m/s^2) 
  Serial.println((String)"SENSOR EVENT\tX: " + event.acceleration.x + " Y: " + event.acceleration.y + " Z: " + event.acceleration.z + " m/s^2");
  Serial.println();
  
  if(DELAY_LOOP_MS > 0){
    delay(DELAY_LOOP_MS);
  }
}

// meter per second per second
void getNormalizeReadings(const Adafruit_LIS3DH& lis3dh, float& x, float& y, float& z){

  // https://learn.adafruit.com/adafruit-lis3dh-triple-axis-accelerometer-breakout/arduino#accelerometer-ranges-1729247-23
  // When reading the range back from the sensor, 0 is ±2g, 1 is ±4g, 2 is ±8g and 3 is ±16g range
  int toRange = -1;
  switch(lis3dh.getRange()){
    case LIS3DH_RANGE_2_G:
    {
      toRange = 2;
      break;
    }
    case LIS3DH_RANGE_4_G:
    {
      toRange = 4;
      break;
    }
    case LIS3DH_RANGE_8_G:
    {
      toRange = 8;
      break;
    }
    case LIS3DH_RANGE_16_G:
    {
      toRange = 16;
      break;
    } 
  }
  
  x = normalize(lis3dh.x, MIN_ACCEL_VAL, MAX_ACCEL_VAL, -toRange, toRange);
  y = normalize(lis3dh.y, MIN_ACCEL_VAL, MAX_ACCEL_VAL, -toRange, toRange);
  z = normalize(lis3dh.z, MIN_ACCEL_VAL, MAX_ACCEL_VAL, -toRange, toRange);
}

// WARNING: there is current a bug in this conversion. Not sure what it is! :(
float normalize(long rawVal, long minFromVal, long maxFromVal, long minToVal, long maxToVal){
  return (minToVal + ((rawVal - minFromVal) / (float)(maxFromVal - minFromVal) * (maxToVal - minToVal))) * SENSORS_GRAVITY_EARTH;
}
