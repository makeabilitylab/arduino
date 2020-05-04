
/**
 * This example prints out a LIS3DH accel values to an OLED screen
 * 
 * Adafruit Gfx Library:
 * https://learn.adafruit.com/adafruit-gfx-graphics-library/overview
 * 
 * Adafruit OLED tutorials:
 * https://learn.adafruit.com/monochrome-oled-breakouts
 * 
 * Adafruit LIS3DH tutorial:
 * https://learn.adafruit.com/adafruit-lis3dh-triple-axis-accelerometer-breakout
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
}

void loop() {
  // put your main code here, to run repeatedly:
  lis.read();      // get X Y and Z data at once
  printAccel();
  delay(100);
}

void printAccel() {
  // Then print out the raw data
  Serial.print(lis.x);
  Serial.print(", ");
  Serial.print(lis.y);
  Serial.print(", ");
  Serial.print(lis.z);
  Serial.println();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("X: ");
  display.println(lis.x);
  display.print("Y: ");
  display.println(lis.y);
  display.print("Z: ");
  display.println(lis.z);
  display.display();
}
