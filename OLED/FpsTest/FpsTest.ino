
/**
 * This program simply tests out the rendering speed of the OLED with
 * the Adafruit Gfx Library
 * 
 * Adafruit Gfx Library:
 * https://learn.adafruit.com/adafruit-gfx-graphics-library/overview
 * 
 * Adafruit OLED tutorials:
 * https://learn.adafruit.com/monochrome-oled-breakouts
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

unsigned long _totalFrameCount = 0;
unsigned long _startTimeStamp = 0;

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
  delay(200);
  display.clearDisplay();

  // setTextSize(size) multiplies the scale of the text by a given integer factor
  // https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
  display.setTextSize(1);
}

void loop() {
  if(_startTimeStamp == 0){
    _startTimeStamp = millis();
  }

  if(_totalFrameCount > 0){
    // clearing the display takes a small FPS hit. We can avoid it
    // if we are drawing over same pixels
    // display.clearDisplay(); 
    display.setCursor(0, 0);
    unsigned long elapsedTime = millis() - _startTimeStamp;
    float fps = _totalFrameCount / (elapsedTime / 1000.0);
    display.print("FPS: ");
    display.print(fps);
  }
  display.display();
  _totalFrameCount++;
}
