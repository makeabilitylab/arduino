/*
 * This example reads in a photocell value from A7 using a voltage-divider circuit and 
 * proportionally sets the brightness of an LED (hooked up to GPIO 21).
 * 
 * This example is modified for the ESP32 from the basic Arduino version covered here:
 * https://makeabilitylab.github.io/physcomp/sensors/photoresistors.html
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

const int LED_OUTPUT_PIN = 21;
const int PHOTOCELL_INPUT_PIN = A7;

// Set the min and max photocell values (this will be based on
// the brightness of your environment and the size of the voltage-divider
// resistor that you selected). So, the best way to set these values
// is to view the photocellVal in the Serial Monitor or Serial Plotter
// under different expected lighting conditions and observe the values
// The comments below assume that the photoresistor is in the R2 position
// in the voltage divider

// Is the photocell R1 or R2 in the voltage divider?
// If it's R1, then we using the "lightness" sensor configuration and the voltage will
// drop with increases in ambient lightness. If the photocell is R2, then we're using the
// "darkness" sensor configuration and the voltage will increase as ambient lightness
// decreases. See: 
// https://makeabilitylab.github.io/physcomp/sensors/photoresistors.html#using-photoresistors-with-microcontrollers
const boolean PHOTOCELL_IS_R2_IN_VOLTAGE_DIVIDER = true; // set false if photocell is R1

// If the photocell is R1, then MIN_PHOTOCELL_VAL is the value of analogRead when it's darkest
// If the photocell is R2, then MIN_PHOTOCELL_VAL is the value of analogRead when it's lightest
// Measure this empirically given your deployment environment and then update this value
// Check the Serial Monitor
const int MIN_PHOTOCELL_VAL = 200; 

// If the photocell is R1, then MAX_PHOTOCELL_VAL is the value of analogRead when it's lightest
// If the photocell is R2, then MAX_PHOTOCELL_VAL is the value of analogRead when it's darkest
// Measure this empirically given your deployment environment and then update this value
const int MAX_PHOTOCELL_VAL = 800; 

const int PWM_CHANNEL = 0; // ESP32 has 16 channels which can generate 16 independent waveforms
const int PWM_FREQ = 500;  // Recall that Arduino Uno is ~490 Hz. Official ESP32 example uses 5,000Hz
const int PWM_RESOLUTION = 8; // We'll use same resolution as Uno (8 bits, 0-255) but ESP32 can go up to 16 bits 

// The max duty cycle value based on PWM resolution (will be 255 if resolution is 8 bits)
const int MAX_DUTY_CYCLE = (int)(pow(2, PWM_RESOLUTION) - 1);

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(PHOTOCELL_INPUT_PIN, INPUT);
  Serial.begin(9600);

  // Setup the PWM waveform and attach our LED output pin
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED_OUTPUT_PIN, PWM_CHANNEL);
}

void loop() {

  // Read the photo-sensitive resistor value. If you have the photocell resistor hooked 
  // up as Rtop in the voltage divider (that is, one leg of the photocell is connected to 5V), 
  // then higher values correspond to brightness. If you have the photocell hooked up as Rbottom 
  // in the voltage divider (that is, one leg of the photocell is connected to GND), then
  // higher values correspond to darkness.
  int photocellVal = analogRead(PHOTOCELL_INPUT_PIN);

  // Remap the value for output. 
  int dutyCycle = map(photocellVal, MIN_PHOTOCELL_VAL, MAX_PHOTOCELL_VAL, 0, 255);

  // The map function does not constrain output outside of the provided range
  // so, we need to make sure that things are within range for the led
  dutyCycle = constrain(dutyCycle, 0, 255);

  if(PHOTOCELL_IS_R2_IN_VOLTAGE_DIVIDER == false){
    // We need to invert the LED (it should be brighter when environment is darker)
    // This assumes the photocell is Rtop in the voltage divider
    dutyCycle = 255 - dutyCycle;
  }

  // Print the raw photocell value and the converted led value (e,g., for Serial 
  // Console and Serial Plotter)
  Serial.print(photocellVal);
  Serial.print(",");
  Serial.println(dutyCycle);

  // Write out the LED value. 
  ledcWrite(PWM_CHANNEL, dutyCycle);

  delay(100); //read from the photocell ~10 times/sec
}
