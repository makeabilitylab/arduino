
/**
   This example prints out a LIS3DH accel values to an OLED screen

   Adafruit Gfx Library:
   https://learn.adafruit.com/adafruit-gfx-graphics-library/overview

   Adafruit OLED tutorials:
   https://learn.adafruit.com/monochrome-oled-breakouts

   Adafruit LIS3DH tutorial:
   https://learn.adafruit.com/adafruit-lis3dh-triple-axis-accelerometer-breakout

   By Jon E. Froehlich
   @jonfroehlich
   http://makeabilitylab.io

   TODO:
   - Could consider using: https://learn.adafruit.com/using-the-adafruit-unified-sensor-driver/how-does-it-work

*/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Used for LIS3DH hardware & software SPI
#define LIS3DH_CS 10
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

int _minSensorVal = -1023;
int _maxSensorVal = 1023;

struct AccelData {
  int x;
  int y;
  int z;
};

const int ARRAY_SIZE = 10;
struct AccelData _accelBuffer[ARRAY_SIZE];
int xData[ARRAY_SIZE];
//int yData[SCREEN_WIDTH];
//int zData[SCREEN_WIDTH];
int _curIndex = 0;

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 0);
  display.println("Screen initialized!");
  display.display();
  delay(3000);

  display.println("Initializing accelerometer...");
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldn't start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");

  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = ");
  Serial.print(2 << lis.getRange());
  Serial.println("G");

  for (int i = 0; i < ARRAY_SIZE; i++) {
    //AccelData accelData = { 0, 0, 0 };
    
//    _accelBuffer[i].x = 0;
//    _accelBuffer[i].y = 0;
//    _accelBuffer[i].z = 0;
    _accelBuffer[i] = { 0, 0, 0};

//    _accelBuffer[i] = 
    
    Serial.println((String)"X: " + _accelBuffer[i].x + " Y: " + _accelBuffer[i].y + " Z: " + _accelBuffer[i].z);
//    xData[i] = 0;
//    yData[i] = 0;
//    zData[i] = 0;
  }
}

void loop() {

  lis.read();      // get X Y and Z data at once
  printAccel();
  if (_curIndex >= ARRAY_SIZE) {
    _curIndex = 0;
    display.clearDisplay();
  }

  xData[_curIndex] = lis.x;
  _accelBuffer[_curIndex].x = lis.x;
  _accelBuffer[_curIndex].y = lis.y;
  _accelBuffer[_curIndex].z = lis.z;
  Serial.print("_accelBuffer[_curIndex].x: ");
  //Serial.println(_accelBuffer[_curIndex].x);
  Serial.println((String)"X: " + _accelBuffer[_curIndex].x + " Y: " + _accelBuffer[_curIndex].y + " Z: " + _accelBuffer[_curIndex].z);
  //AccelData accelData = { lis.x, lis.y, lis.z };
 
  //_accelBuffer[_curIndex] = { lis.x, lis.y, lis.z };
//  _accelBuffer[_curIndex].x = lis.x;
//  _accelBuffer[_curIndex].y = lis.y;
//  _accelBuffer[_curIndex].z = lis.z;
//  Serial.println((String)"X: " + _accelBuffer[_curIndex].x + " Y: " + _accelBuffer[_curIndex].y + " Z: " + _accelBuffer[_curIndex].z);

//  for(int x = 1; x < _curIndex; x++){
//    AccelData accelData1 = _accelBuffer[x - 1];
//    AccelData accelData2 = _accelBuffer[x];
//    float yPixelVal1 = getYPixelFromSensorVal(accelData1.x);
//    float yPixelVal2 = getYPixelFromSensorVal(accelData2.x);
//
//    // void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
//    // https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#drawing-lines-3-4
//    display.drawLine(x - 1, yPixelVal1, x, yPixelVal2, SSD1306_WHITE);
//  }



  display.display();
  _curIndex++;
  delay(100);
}

/**
   Converts a sensor value to a y-pixel value and returns the y-pixel value
*/
float getYPixelFromSensorVal(float sensorVal) {
  return map(sensorVal, _minSensorVal, _maxSensorVal, display.height(), 0);
}

void printAccel() {
  // Then print out the raw data
  Serial.print(lis.x);
  Serial.print(", ");
  Serial.print(lis.y);
  Serial.print(", ");
  Serial.print(lis.z);
  Serial.println();
}
