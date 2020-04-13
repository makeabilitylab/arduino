/*
 * This example flashes through a predefined sequence of colors 
 * using an RGB LED and digitalWrite. Make sure to correctly set
 * IS_COMMON_ANODE to true (if you're using a common anode RGB LED)
 * or to false (if you're using a common cathode RGB LED).
 * 
 * By Jon Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * For a walkthrough and circuit diagram, see:
 * https://makeabilitylab.github.io/physcomp/arduino/rgb-led
 * 
 */

// Change this to based on whether you are using a common anode or common cathode
// RGB LED. See: https://makeabilitylab.github.io/physcomp/arduino/rgb-led 
// If you are working with a common cathode RGB LED, set this to false.
// Note Adafruit sells a common anode design: https://www.adafruit.com/product/159
const boolean IS_COMMON_ANODE = false; 

const int RGB_RED_PIN = 6;
const int RGB_GREEN_PIN = 5;
const int RGB_BLUE_PIN = 3;
const int DELAY_MS = 1000; // delay in ms between changing colors

void setup() {
  // Set the RGB pins to output
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // Turn on Serial so we can verify expected colors via Serial Monitor
  Serial.begin(9600); 
}

void loop() {

  // red
  Serial.print("Red: ");
  setRgbLedColor(HIGH, LOW, LOW);
  delay(DELAY_MS);
  
  // green
  Serial.print("Green: ");
  setRgbLedColor(LOW, HIGH, LOW);
  delay(DELAY_MS);
  
  // blue
  Serial.print("Blue: ");
  setRgbLedColor(LOW, LOW, HIGH);
  delay(DELAY_MS);
  
  // purple
  Serial.print("Purple: ");
  setRgbLedColor(HIGH, LOW, HIGH);
  delay(DELAY_MS);
  
  // turqoise
  Serial.print("Turqoise: ");
  setRgbLedColor(LOW, HIGH, HIGH);
  delay(DELAY_MS);
  
  // yellow
  Serial.print("Yellow: ");
  setRgbLedColor(HIGH, HIGH, LOW);
  delay(DELAY_MS);
  
  // white
  Serial.print("White: ");
  setRgbLedColor(HIGH, HIGH, HIGH);
  delay(DELAY_MS);
}


/**
 * setRgbLedColor takes in either HIGH or LOW for red, green, and blue.
 * 
 * This function is based on https://gist.github.com/jamesotron/766994
 */
void setRgbLedColor(int red, int green, int blue)
{
  // If a common anode LED, invert values
  if(IS_COMMON_ANODE == true){
    red = !red;
    green = !green;
    blue = !blue;
  }

  Serial.print(red);
  Serial.print(", ");
  Serial.print(green);
  Serial.print(", ");
  Serial.println(blue);
  
  digitalWrite(RGB_RED_PIN, red);
  digitalWrite(RGB_GREEN_PIN, green);
  digitalWrite(RGB_BLUE_PIN, blue);  
}
