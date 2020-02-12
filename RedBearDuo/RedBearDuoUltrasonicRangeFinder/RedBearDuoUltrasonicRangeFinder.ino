/*
 * This example shows how to use the HC-SR04 Ultrasonic Sensor, which we purchased
 * from Sparkfun: https://www.sparkfun.com/products/13959. According to Sparkfun, 
 * the HC-SR04 runs on 5V with an operating current of 15mA; however, in our tests,
 * we were able to get it to reliably work with our 3.3V boards.
 * 
 * Originally by Sparkfun Aug 3, 2016
 * https://github.com/sparkfun/HC-SR04_UltrasonicSensor
 * 
 * Modified by Jon Froehlich May 8, 2018
 * http://makeabilitylab.io
 * 
 * See also:
 *  - http://www.instructables.com/id/Simple-Arduino-and-HC-SR04-Example/
 *  - https://www.arduino.cc/en/Tutorial/Ping
 *  - https://howtomechatronics.com/tutorials/arduino/ultrasonic-sensor-hc-sr04/
 */

/* 
 * IMPORTANT: When working with the RedBear Duo, you must have this line of
 * code at the top of your program. The default state is SYSTEM_MODE(AUTOMATIC);
 * however, this puts the RedBear Duo in a special cloud-based mode that we 
 * are not using. For our purposes, set SYSTEM_MODE(SEMI_AUTOMATIC) or
 * SYSTEM_MODE(MANUAL). See https://docs.particle.io/reference/firmware/photon/#system-modes
 */
SYSTEM_MODE(MANUAL); 

// Pins
const int TRIG_PIN = D0;
const int ECHO_PIN = D1;

// Anything over 400 cm (23200 us pulse) is "out of range"
const unsigned int MAX_DIST = 23200;

void setup() {

  // The Trigger pin will tell the sensor to range find
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);

  // We'll use the serial monitor to view the sensor output
  Serial.begin(9600);
}

void loop() {

  unsigned long t1;
  unsigned long t2;
  unsigned long pulse_width;
  float cm;
  float inches;

  // Hold the trigger pin high for at least 10 us
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Wait for pulse on echo pin
  while ( digitalRead(ECHO_PIN) == 0 );

  // Measure how long the echo pin was held high (pulse width)
  // Note: the micros() counter will overflow after ~70 min
  // TODO: We probably need to check for a timeout here just in case
  // the ECHO_PIN never goes HIGH... so like
  // while ( digitalRead(ECHO_PIN) == 1 && micros() - t1 < threshold);
  t1 = micros();
  while ( digitalRead(ECHO_PIN) == 1);
  t2 = micros();
  pulse_width = t2 - t1;

  // Calculate distance in centimeters and inches. The constants
  // are found in the datasheet, and calculated from the assumed speed 
  // of sound in air at sea level (~340 m/s).
  // Datasheet: https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
  cm = pulse_width / 58.0;
  inches = pulse_width / 148.0;

  // Print out results
  if ( pulse_width > MAX_DIST ) {
    Serial.println("Out of range");
  } else {
    Serial.print(cm);
    Serial.print(" cm \t");
    Serial.print(inches);
    Serial.println(" in");
  }
  
  // The HC-SR04 datasheet recommends waiting at least 60ms before next measurement
  // in order to prevent accidentally noise between trigger and echo
  // See: https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
  delay(60);
}
