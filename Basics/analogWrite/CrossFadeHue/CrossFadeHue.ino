/*
 * This example cross fades the hue, which is then converted to RGB.
 * 
 * Uses an RGB to HSL and HSL to RGB conversion library from here:
 *   https://github.com/ratkins/RGBConverter
 *   
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 * Adapted from https://learn.adafruit.com/adafruit-arduino-lesson-3-rgb-leds?view=all
 */

#include "src/RGBConverter/RGBConverter.h"

// Change this to 0 if you are working with a common cathode RGB LED
// We purchased Common Anode RGB LEDs for class: https://www.adafruit.com/product/159
const boolean COMMON_ANODE = true; 

const int RGB_RED_PIN = 6;
const int RGB_GREEN_PIN  = 5;
const int RGB_BLUE_PIN  = 3;
const int DELAY_INTERVAL = 10; // interval in ms between incrementing hues
float _hue = 0; //hue varies between 0 - 1
float _step = 0.001f;

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
 
  byte rgb[3];
  _rgbConverter.hslToRgb(_hue,1.0,0.5,rgb);

  Serial.print("hue=");
  Serial.print(_hue);
  Serial.print(" r=");
  Serial.print(rgb[0]);
  Serial.print(" g=");
  Serial.print(rgb[1]);
  Serial.print(" b=");
  Serial.println(rgb[2]);
  
  setColor(rgb[0], rgb[1], rgb[2]); 

  _hue += _step;
  if(_hue > 1.0){
    _hue = 0;
  }

  delay(DELAY_INTERVAL);
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
