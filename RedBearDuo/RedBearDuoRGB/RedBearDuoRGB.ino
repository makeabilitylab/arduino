/*
 * This example changes the colors of the RGB LED
 * 
 * By Jon Froehlich for CSE590
 * http://makeabilitylab.io
 * 
 * Adapted from https://learn.adafruit.com/adafruit-arduino-lesson-3-rgb-leds?view=all
 */

/* 
 * IMPORTANT: When working with the RedBear Duo, you must have this line of
 * code at the top of your program. The default state is SYSTEM_MODE(AUTOMATIC);
 * however, this puts the RedBear Duo in a special cloud-based mode that we 
 * are not using. For our purposes, set SYSTEM_MODE(SEMI_AUTOMATIC) or
 * SYSTEM_MODE(MANUAL). See https://docs.particle.io/reference/firmware/photon/#system-modes
 */
SYSTEM_MODE(MANUAL); 

#define COMMON_ANODE 1 // change this to 0 if you are working with a common cathode RGB LED

const int RGB_RED_PIN = D0;
const int RGB_GREEN_PIN  = D1;
const int RGB_BLUE_PIN  = D2;
const int DELAY = 500; // delay between changing colors

void setup() {
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
}

void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(RGB_RED_PIN, red);
  analogWrite(RGB_GREEN_PIN, green);
  analogWrite(RGB_BLUE_PIN, blue);  
}




