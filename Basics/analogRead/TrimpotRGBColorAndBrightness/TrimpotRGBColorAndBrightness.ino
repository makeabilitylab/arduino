/*
 * This example reads in potentiometers from A0 and A1 to set the color and brightness 
 * of an RGB LED. See also FadeHSL for another example using the HSL color space.
 * 
 * Uses an RGB to HSL and HSL to RGB conversion library from here:
 *   https://github.com/ratkins/RGBConverter
 *   
 * By Jon Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * Adapted from https://learn.adafruit.com/adafruit-arduino-lesson-3-rgb-leds?view=all
 */

#include "src/RGBConverter/RGBConverter.h"
//#include "RGBConverter.h"

// Change this to false if you are working with a common cathode RGB LED
// We purchased Common Anode RGB LEDs for class: https://www.adafruit.com/product/159
const boolean COMMON_ANODE = true; 

const int RGB_RED_PIN = 6;
const int RGB_GREEN_PIN  = 5;
const int RGB_BLUE_PIN  = 3;
const int POT_HUE_PIN = A0;
const int POT_BRIGHTNESS_PIN = A1;
const int INTERVAL = 10; // interval in ms between incrementing hues

RGBConverter _rgbConverter;

void setup() {
  // Set the RGB pins to output
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // Turn on Serial so we can verify expected colors via Serial Monitor
  Serial.begin(9600);   
}

void loop() {

  // read the potentiometers
  int potHueVal = analogRead(POT_HUE_PIN);
  int potBrightnessVal = analogRead(POT_BRIGHTNESS_PIN);

  // convert potentiometer value to a fraction between 0 and 1 where
  // 1 is brightest (1023 is the maximum analog input value).
  float hue = potHueVal / 1023.0;
  float brightness = potBrightnessVal / 1023.0; 
  
  byte rgb[3];
  _rgbConverter.hslToRgb(hue,1.0,brightness,rgb);

  Serial.print("hue=");
  Serial.print(hue);
  Serial.print(" brightness=");
  Serial.print(brightness);
  Serial.print(" r=");
  Serial.print(rgb[0]);
  Serial.print(" g=");
  Serial.print(rgb[1]);
  Serial.print(" b=");
  Serial.println(rgb[2]);
  
  setColor(rgb[0], rgb[1], rgb[2]);  

  delay(INTERVAL);
}

/**
 * setColor takes in values between 0 - 255 for the amount of red, green, and blue, respectively
 * where 255 is the maximum amount of that color and 0 is none of that color. You can illuminate
 * all colors by intermixing different combinations of red, green, and blue. 
 * 
 * This function is based on https://gist.github.com/jamesotron/766994
 * 
 */
void setColor(int red, int green, int blue)
{
  // If a common anode LED, invert values
  if(COMMON_ANODE == true){
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  }
  analogWrite(RGB_RED_PIN, red);
  analogWrite(RGB_GREEN_PIN, green);
  analogWrite(RGB_BLUE_PIN, blue);  
}
