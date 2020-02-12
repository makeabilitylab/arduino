/*
 * This example changes the colors of the RGB LED using digitalWrite
 * 
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 */

const int RGB_RED_PIN = 6;
const int RGB_GREEN_PIN  = 5;
const int RGB_BLUE_PIN  = 3;
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

  // Because the RGB LED we purchased for the class is a 'common anode'
  // RGB, the way we turn on each light is counter to our intuition
  // We set a pin to LOW to turn on the corresponding LED to its maximum brightness
  // and HIGH to turn it off (the opposite of what you might think)

  // Set the RGB LED to red
  Serial.println("Color=Red (LOW, HIGH, HIGH)");
  digitalWrite(RGB_RED_PIN, LOW);     // turn on the red LED
  digitalWrite(RGB_GREEN_PIN, HIGH);  // turn off the green LED
  digitalWrite(RGB_BLUE_PIN, HIGH);   // turn off the blue LED
  delay(DELAY);

  // Set the RGB LED to green
  Serial.println("Color=Green (HIGH, LOW, HIGH)");
  digitalWrite(RGB_RED_PIN, HIGH);    // turn off the red LED
  digitalWrite(RGB_GREEN_PIN, LOW);   // turn on the green LED
  digitalWrite(RGB_BLUE_PIN, HIGH);   // turn on the blue LED
  delay(DELAY);

  // Set the RGB LED to blue
  Serial.println("Color=Blue (HIGH, HIGH, LOW)");
  digitalWrite(RGB_RED_PIN, HIGH);    // turn off the red LED
  digitalWrite(RGB_GREEN_PIN, HIGH);  // turn off the green LED
  digitalWrite(RGB_BLUE_PIN, LOW);    // turn on the blue LED
  delay(DELAY);

  // Set the RGB LED to yellow (by turning on green and blue!)
  Serial.println("Color=Yellow (255, LOW, LOW)");
  digitalWrite(RGB_RED_PIN, HIGH);    // turn off the red LED
  digitalWrite(RGB_GREEN_PIN, LOW);   // turn on the green LED
  digitalWrite(RGB_BLUE_PIN, LOW);    // turn on the blue LED
  delay(DELAY);

  // Set the RGB LED to purple (by turning on red and blue!)
  Serial.println("Color=Purple (LOW, HIGH, LOW)");
  digitalWrite(RGB_RED_PIN, LOW);     // turn on the red LED
  digitalWrite(RGB_GREEN_PIN, HIGH);  // turn off the green LED
  digitalWrite(RGB_BLUE_PIN, LOW);    // turn on the blue LED
  delay(DELAY);
}
