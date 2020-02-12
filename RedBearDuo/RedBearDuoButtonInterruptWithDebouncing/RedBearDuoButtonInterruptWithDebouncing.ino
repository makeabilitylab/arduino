/*
 * This is a push button example with interrupts and debouncing.
 * 
 * The circuit:
 *  - internal LED on D7
 *  - pushbutton connected to D1 and 3V3
 * 
 * Originally by Bjoern Hartmann
 * https://github.com/bjo3rn/idd-examples/tree/master/redbearduo/examples/button_interrupt_debounce
 * 
 * Modified by Jon Froehlich for CSE590
 * http://makeabilitylab.io
 * 
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

// The official Particle Photon documentation states that external interrupts
// should work on all pins except for D0 and A5, see:
// https://docs.particle.io/reference/firmware/photon/#interrupts
const int BUTTON_INPUT_PIN = D8; 

long _debounceDelay = 50;    // the debounce time in ms

// From the Particle Photon documentation:
//   "Typically global variables are used to pass data between an ISR and 
//   the main program. To make sure variables shared between an ISR and the 
//   main program are updated correctly, declare them as volatile."
// See: https://docs.particle.io/reference/firmware/photon/#interrupts
volatile int _ledState = LOW;

// Function declaration for interrupts
void toggle(void);
void isr(void);

// We will use a software timer to help us debounce. 
// See: https://docs.particle.io/reference/firmware/photon/#software-timers
Timer timer(_debounceDelay, toggle, true); // oneshot timer so last param=true

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(BUTTON_INPUT_PIN, INPUT_PULLDOWN);

  // Hook up interrupt to button again but this time call the
  // ISR routine, which will start a timer on every rising edge
  // and after a set delay (_debounceDelay), the timer callback
  // is called (which is set to toggle())
  attachInterrupt(BUTTON_INPUT_PIN, isr, CHANGE); //CHANGE, RISING or FALLING
}

void loop() {
  // intentionally empty
}

// Called automatically when a button state change occurs
void isr(){
  noInterrupts(); // temporarily pause the interrupts
  if(digitalRead(BUTTON_INPUT_PIN) == HIGH){
    timer.resetFromISR(); //start or reset timer on every rising edge
  } else {
    timer.stopFromISR(); //stop on falling edge
  } 
  interrupts(); // re-enable interrupts
}

// Called by the software timer after _buttonDelay. 
void toggle(){
  // If the button state is still high after _buttonDelay
  // then toggle the state
  if(digitalRead(BUTTON_INPUT_PIN) == HIGH){
    _ledState = !_ledState;
    digitalWrite(LED_OUTPUT_PIN, _ledState);
  }
}

