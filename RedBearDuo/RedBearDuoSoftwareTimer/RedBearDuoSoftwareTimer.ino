/*
 * This example counts seconds and toggles an LED on/off every second
 * using a software timer
 * 
 * 
 * By Jon Froehlich for CSE590
 * http://makeabilitylab.io
 * 
 * Based on example code from:
 * https://docs.particle.io/reference/firmware/photon/#software-timers
 */ 

/* 
 * IMPORTANT: When working with the RedBear Duo, you must have this line of
 * code at the top of your program. The default state is SYSTEM_MODE(AUTOMATIC);
 * however, this puts the RedBear Duo in a special cloud-based mode that we 
 * are not using. For our purposes, set SYSTEM_MODE(SEMI_AUTOMATIC) or
 * SYSTEM_MODE(MANUAL). See https://docs.particle.io/reference/firmware/photon/#system-modes
 */
SYSTEM_MODE(MANUAL);

// This is the internal LED pin on the Duo
const int LED_OUTPUT_PIN = D7; 

// From the Particle Photon documentation:
//   "Typically global variables are used to pass data between an ISR and 
//   the main program. To make sure variables shared between an ISR and the 
//   main program are updated correctly, declare them as volatile."
// See: https://docs.particle.io/reference/firmware/photon/#interrupts
volatile int _ledState = LOW;
volatile int _seconds = 0;

void timer_callback(void); // function declaration for interrupt

Timer _timer(1000, timer_callback);

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  Serial.begin(9600);
  _timer.start();
}

void loop() {
  // intentionally empty
}

// called automatically by software timer
void timer_callback(){
  _ledState = !_ledState;
  digitalWrite(LED_OUTPUT_PIN, _ledState);
  Serial.println(++_seconds);
}
