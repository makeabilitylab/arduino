/*
 * This example reads in a potentiometer value (from A0) and sets the brightness 
 * of an RGB LED. While more straightforward than TrimpotRGBBrightness, it uses
 * delay() function calls which makes the prototype seem less responsive to changes
 * in the potentiometer.
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
const int DELAY = 1000; // delay in ms between changing colors

void setup() {
  // Set the RGB pins to output
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // Turn on Serial so we can verify expected colors via Serial Monitor
  Serial.begin(9600); 
}

void loop() {

  Serial.println("Color=Red");
  setColor(255, 0, 0);  // red
  delay(DELAY);

  Serial.println("Color=Green");
  setColor(0, 255, 0);  // green
  delay(DELAY);

  Serial.println("Color=Blue");
  setColor(0, 0, 255);  // blue
  delay(DELAY);

  Serial.println("Color=Yellow");
  setColor(255, 255, 0);  // yellow
  delay(DELAY);  

  Serial.println("Color=Purple");
  setColor(80, 0, 80);  // purple
  delay(DELAY);

  Serial.println("Color=Aqua");
  setColor(0, 255, 255);  // aqua
  delay(DELAY);

  Serial.println("Color=White");
  setColor(255, 255, 255);  // white
  delay(DELAY);

  Serial.println("Color=Off");
  setColor(0, 0, 0);  // white
  delay(DELAY);
}

/**
 * setColor takes in values between 0 - 255 for the amount of red, green, and blue, respectively
 * where 255 is the maximum amount of that color and 0 is none of that color. You can illuminate
 * all colors by intermixing different combinations of red, green, and blue. The function also
 * reads from POT_INPUT_PIN to retrieve a brightness value
 * 
 * This function is based on https://gist.github.com/jamesotron/766994
 * 
 */
void setColor(int red, int green, int blue)
{
  // read the potentiometer value. we have to keep reading this value
  // on every setColor call in case it changed
  int potVal = analogRead(POT_INPUT_PIN);

  // convert potentiometer value to a fraction between 0 and 1 where
  // 1 is brightest (1023 is the maximum analog input value).
  float brightness = potVal / 1023.0; 
  
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
