/*
 * This example cross fades the colors of the RGB LED
 * 
 * By Jon Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * For a walkthrough and circuit diagram, see:
 * https://makeabilitylab.github.io/physcomp/arduino/rgb-led-fade
 * 
 * Based, in part, on:
 * - https://learn.adafruit.com/adafruit-arduino-lesson-3-rgb-leds?view=all
 * - https://gist.github.com/jamesotron/766994 (link no longer available)
 */

// Change this to based on whether you are using a common anode or common cathode
// RGB LED. See: https://makeabilitylab.github.io/physcomp/arduino/rgb-led 
// If you are working with a common cathode RGB LED, set this to false.
// Note Adafruit sells a common anode design: https://www.adafruit.com/product/159
const boolean COMMON_ANODE = false; 

const int RGB_RED_PIN = 6;
const int RGB_GREEN_PIN  = 5;
const int RGB_BLUE_PIN  = 3;
const int DELAY_MS = 20; // delay in ms between changing colors
const int MAX_COLOR_VALUE = 255;

enum RGB{
  RED,
  GREEN,
  BLUE,
  NUM_COLORS
};

int _rgbLedValues[] = {255, 0, 0}; // Red, Green, Blue
enum RGB _curFadingUpColor = GREEN;
enum RGB _curFadingDownColor = RED;
const int FADE_STEP = 5;  

void setup() {
  // Set the RGB pins to output
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // Turn on Serial so we can verify expected colors via Serial Monitor
  Serial.begin(9600); 
  Serial.println("Red, Green, Blue");

  // Set initial color
  setColor(_rgbLedValues[RED], _rgbLedValues[GREEN], _rgbLedValues[BLUE]);
  delay(DELAY_MS);
}

void loop() {

  // Increment and decrement the RGB LED values for the current
  // fade up color and the current fade down color
  _rgbLedValues[_curFadingUpColor] += FADE_STEP;
  _rgbLedValues[_curFadingDownColor] -= FADE_STEP;

  // Check to see if we've reached our maximum color value for fading up
  // If so, go to the next fade up color (we go from RED to GREEN to BLUE
  // as specified by the RGB enum)
  // This fade code partially based on: https://gist.github.com/jamesotron/766994
  if(_rgbLedValues[_curFadingUpColor] > MAX_COLOR_VALUE){
    _rgbLedValues[_curFadingUpColor] = MAX_COLOR_VALUE;
    _curFadingUpColor = (RGB)((int)_curFadingUpColor + 1);

    if(_curFadingUpColor > (int)BLUE){
      _curFadingUpColor = RED;
    }
  }

  // Check to see if the current LED we are fading down has gotten to zero
  // If so, select the next LED to start fading down (again, we go from RED to 
  // GREEN to BLUE as specified by the RGB enum)
  if(_rgbLedValues[_curFadingDownColor] < 0){
    _rgbLedValues[_curFadingDownColor] = 0;
    _curFadingDownColor = (RGB)((int)_curFadingDownColor + 1);

    if(_curFadingDownColor > (int)BLUE){
      _curFadingDownColor = RED;
    }
  }

  // Set the color and then delay
  setColor(_rgbLedValues[RED], _rgbLedValues[GREEN], _rgbLedValues[BLUE]);
  delay(DELAY_MS);
}

/**
 * setColor takes in values between 0 - 255 for the amount of red, green, and blue, respectively
 * where 255 is the maximum amount of that color and 0 is none of that color. You can illuminate
 * all colors by intermixing different combinations of red, green, and blue
 * 
 * This function is based on https://gist.github.com/jamesotron/766994
 */
void setColor(int red, int green, int blue)
{
  Serial.print(red);
  Serial.print(", ");
  Serial.print(green);
  Serial.print(", ");
  Serial.println(blue);

  // If a common anode LED, invert values
  if(COMMON_ANODE == true){
    red = MAX_COLOR_VALUE - red;
    green = MAX_COLOR_VALUE - green;
    blue = MAX_COLOR_VALUE - blue;
  }
  analogWrite(RGB_RED_PIN, red);
  analogWrite(RGB_GREEN_PIN, green);
  analogWrite(RGB_BLUE_PIN, blue);  
}
