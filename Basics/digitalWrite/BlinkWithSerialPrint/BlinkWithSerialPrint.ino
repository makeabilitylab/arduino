/*
 * Turns on and off pin 3 once a second and demonstrates Serial.print, which
 * uses the serial protocol to communicate data (as ASCII text) back to 
 * your computer via the USB cable.
 * 
 * To see the incoming serial data, in the Arduino IDE, go to Tools -> Serial Monitor
 * 
 * Reference:
 * https://www.arduino.cc/reference/en/language/functions/communication/serial/print/
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

// While not necessary, it's a good idea to use const variables
// for constants like specifying pins that you use.
const int LED_OUTPUT_PIN = 3;

// The setup function runs once when you press reset or power the board
void setup() {
  // On the Arduino Uno, Leonardo, etc. pins 0 - 13 can either be input or output.
  // We must specify how we're using the pin by using pinMode. In this case, we want to
  // control an LED, so set the pin to OUTPUT
  pinMode(LED_OUTPUT_PIN, OUTPUT);

  // You must initialize the serial port with a baud rate
  // In this case, the ferociously fast (not really) 9600 bps or 9.6 kilobits/second :)
  Serial.begin(9600);
}

// The loop function runs over and over again forever
void loop() {
  
  // Turn on the LED
  digitalWrite(LED_OUTPUT_PIN, HIGH);   // turn LED on (5V)
  Serial.println("Pin 3 is HIGH (5V)"); // print status
  delay(500);                           // delay is in milliseconds
  
  // Turn off the LED
  digitalWrite(LED_OUTPUT_PIN, LOW);    // turn LED off (0V)
  Serial.println("Pin 3 is LOW (0V)");  // print status
  delay(500);                           // delay is in milliseconds
}
