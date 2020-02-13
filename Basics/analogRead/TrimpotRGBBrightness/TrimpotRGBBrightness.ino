/*
 * This example reads in a potentiometer value (from A0) and sets the brightness 
 * of an RGB LED. For a simpler though less responsive version of this program, see
 * TrimpotRGBBrightnessSimple.
 * 
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 * Adapted from https://learn.adafruit.com/adafruit-arduino-lesson-3-rgb-leds?view=all
 */

// Change this to 0 if you are working with a common cathode RGB LED
// We purchased Common Anode RGB LEDs for class: https://www.adafruit.com/product/159
const boolean COMMON_ANODE = true; 

const int RGB_RED_PIN = 6;
const int RGB_GREEN_PIN  = 5;
const int RGB_BLUE_PIN  = 3;
const int POT_INPUT_PIN = A0;
const int INTERVAL = 1000; // interval in ms between changing colors

long _timestampLastColorChange = 0; 

int _colors[][3] = { 
                     {255,0,0}, // red
                     {0,255,0}, // green
                     {0,0,255},   // blue
                     {255,255,0}, // yellow
                     {255,0,255}, // purple
                     {0,255,255}, // aqua
                   };

int NUM_COLORS = 6;
                   
int _curColorIndex = 0; // used to loop through colors

void setup() {
  // Set the RGB pins to output
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // Turn on Serial so we can verify expected colors via Serial Monitor
  Serial.begin(9600); 
}

void loop() {

  // read the potentiometer value
  int potVal = analogRead(POT_INPUT_PIN);

  // convert potentiometer value to a fraction between 0 and 1 where
  // 1 is brightest (1023 is the maximum analog input value).
  float brightness = potVal / 1023.0; 

  setColor(_colors[_curColorIndex][0], _colors[_curColorIndex][1], _colors[_curColorIndex][2], brightness);  

  // We *don't* use delay() function calls here because delays cause our
  // program to wait and not do anything. But we want our prototype to be responsive!
  // We want the RGB LED brightness to instantly respond to changes in the potentiometer
  // So, we call loop() as fast as possible with no delays and just track how long
  // we've been in each color state--once we've passed the INTERVAL threshold, we move
  // to the next color
  unsigned long timestampInMillis = millis();
  if(timestampInMillis - _timestampLastColorChange > INTERVAL) {
    _curColorIndex = _curColorIndex + 1;
    _timestampLastColorChange = timestampInMillis;
  }

  if(_curColorIndex >= NUM_COLORS){
    _curColorIndex = 0;
  }
}

/**
 * setColor takes in values between 0 - 255 for the amount of red, green, and blue, respectively
 * where 255 is the maximum amount of that color and 0 is none of that color. You can illuminate
 * all colors by intermixing different combinations of red, green, and blue. The function also takes
 * in a brightness value between 0 and 1
 * 
 * This function is based on https://gist.github.com/jamesotron/766994
 * 
 */
void setColor(int red, int green, int blue, float brightness)
{
   
  // set the color brightness
  red = red * brightness; 
  green = green * brightness; 
  blue = blue * brightness; 
 
  Serial.print("Setting color to");
  Serial.print(" RED=");
  Serial.print(red);
  Serial.print(" GREEN=");
  Serial.print(green);
  Serial.print(" BLUE=");
  Serial.println(blue);

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
