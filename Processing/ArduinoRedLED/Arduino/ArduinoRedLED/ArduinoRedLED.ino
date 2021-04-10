/*
 * Sends a brightness value as a float [0, 1] over the serial port. Intended to
 * work with the Processing program ArduinoRedLED.pde
 * 
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 */

const int LED_PIN = 3;
const int BRIGHTNESS_CONTROL_PIN = A0;
const int DELAY_INTERVAL = 50; // interval in ms between incrementing hues

void setup() {
  // Set the LED pin to output
  pinMode(LED_PIN, OUTPUT);

  // Turn on Serial so we can send our data to the Processing program
  Serial.begin(9600);   
}

void loop() {

  // get the brightness value from analog input [0, 1023]
  int brightnessVal = analogRead(BRIGHTNESS_CONTROL_PIN);
  float brightnessPercentage = brightnessVal / 1023.0; // convert to a percentage

  // send data to Processing program over Serial. The '4' specifies the number
  // of decimal places to use, see https://www.arduino.cc/en/serial/print
  Serial.println(brightnessPercentage, 4); 

  // set the LED brightness based on analog input
  int ledBrightness = map(brightnessVal, 0, 1023, 0, 255); // convert to [0, 255]
  analogWrite(LED_PIN, ledBrightness);

  delay(DELAY_INTERVAL);
}
