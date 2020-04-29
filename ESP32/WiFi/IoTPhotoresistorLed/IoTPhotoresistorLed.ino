/*
 * This example reads in a photoresistor value from A7 using a voltage-divider circuit, 
 * proportionally sets the brightness of an LED (hooked up to GPIO 21), and uploads
 * the raw photoresistor value to the cloud using Adafruit IO.
 * 
 * WARNING: You cannot use the ESP32's ADC#2 with WiFi active. You can only use
 * ADC#1 pins. See below for details.
 * 
 * This code is based, in part, on:
 * https://makeabilitylab.github.io/physcomp/sensors/photoresistors.html
 * https://learn.adafruit.com/adafruit-io-basics-analog-input
 * 
 * The Adafruit IO tutorials:
 * https://learn.adafruit.com/welcome-to-adafruit-io/overview
 * 
 * The Adafruit IO source code:
 * https://github.com/adafruit/Adafruit_IO_Arduino/tree/master/src
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

/******************** Photoresistor / LED Circuit ***************************/

const int LED_OUTPUT_PIN = 21;

// WARNING! Because this program uses WiFi, we cannot use ADC#2, which
// affects A5(4), A11 (12), A12 (13), A6 (14), A8 (15), A1 (25), A0 (26), A10 (27)
// (The GPIO pins are in parantheses). You must use ADC#1 instead, which are
// pins A7 (32), A9 (33), A2 (34), A4 (36), and A3 (39)
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
const boolean PHOTOCELL_IS_R2_IN_VOLTAGE_DIVIDER = false; // set false if photocell is R1

// If the photocell is R1, then MIN_PHOTOCELL_VAL is the value of analogRead when it's darkest
// If the photocell is R2, then MIN_PHOTOCELL_VAL is the value of analogRead when it's lightest
// Measure this empirically given your deployment environment and then update this value
// Check the Serial Monitor
const int MIN_PHOTOCELL_VAL = 500; 

// If the photocell is R1, then MAX_PHOTOCELL_VAL is the value of analogRead when it's lightest
// If the photocell is R2, then MAX_PHOTOCELL_VAL is the value of analogRead when it's darkest
// Measure this empirically given your deployment environment and then update this value
const int MAX_PHOTOCELL_VAL = 3500; 

const int PWM_CHANNEL = 0; // ESP32 has 16 channels which can generate 16 independent waveforms
const int PWM_FREQ = 500;  // Recall that Arduino Uno is ~490 Hz. Official ESP32 example uses 5,000Hz
const int PWM_RESOLUTION = 8; // We'll use same resolution as Uno (8 bits, 0-255) but ESP32 can go up to 16 bits 

// The max duty cycle value based on PWM resolution (will be 255 if resolution is 8 bits)
const int MAX_DUTY_CYCLE = (int)(pow(2, PWM_RESOLUTION) - 1);

/************************** Adafruit IO stuff ***********************************/

// The name of your feed here will show up in your feeds:
// So, for example: https://io.adafruit.com/makeabilitylab/feeds
AdafruitIO_Feed *_adafruitIoFeed= io.feed("lightlevel");

unsigned long _lastUploadTimestamp = 0;
int _lastUploadedVal = -1;
const unsigned long MAX_ELAPSED_TIME_BETWEEN_UPLOADS_MS = 10000; // max upload rate on free tier is 30/minute 
const unsigned long MIN_ELAPSED_TIME_BETWEEN_UPLOADS_MS = 2000; // don't set this lower than 2000 (2 secs) given upload rate limit

void setup() {
  Serial.begin(9600);

  // Setup our pins
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(PHOTOCELL_INPUT_PIN, INPUT);
  
  // Setup the PWM waveform and attach our LED output pin
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED_OUTPUT_PIN, PWM_CHANNEL);

  // Wait for serial monitor to open
  while(!Serial);

  // Connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // Wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // We are connected
  Serial.println();
  Serial.println(io.statusText());
}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  // Read the photo-sensitive resistor value. If you have the photocell resistor hooked 
  // up as Rtop in the voltage divider (that is, one leg of the photocell is connected to 5V), 
  // then higher values correspond to brightness. If you have the photocell hooked up as Rbottom 
  // in the voltage divider (that is, one leg of the photocell is connected to GND), then
  // higher values correspond to darkness.
  int photocellVal = analogRead(PHOTOCELL_INPUT_PIN);
  unsigned long currentTimestamp = millis();

  // Check to see if the photocell values are different. If they are different, make sure we've passed the 
  // MIN_ELAPSED_TIME_BETWEEN_UPLOADS_MS threshold as we need to throttle our uploads. If they are
  // the same, then we need to make sure we've passed the MAX_ELAPSED_TIME_BETWEEN_UPLOADS_MS threshold
  if ((_lastUploadedVal != photocellVal && (currentTimestamp - _lastUploadTimestamp) > MIN_ELAPSED_TIME_BETWEEN_UPLOADS_MS) || 
      (_lastUploadedVal == photocellVal && (currentTimestamp - _lastUploadTimestamp) > MAX_ELAPSED_TIME_BETWEEN_UPLOADS_MS)){

    if(_lastUploadedVal != photocellVal){
      Serial.println((String)"Read in new value: " + photocellVal + " Last uploaded val: " + _lastUploadedVal);
      Serial.println((String)"Time since last upload: " + (currentTimestamp - _lastUploadTimestamp) + " ms"); 
    }else{
      Serial.print((String)"Photoresistor is steady at " + photocellVal);
      Serial.print((String)" but time since last upload is " + (currentTimestamp - _lastUploadTimestamp) + " ms");
      Serial.println((String)", which exceeds threshold of " + MAX_ELAPSED_TIME_BETWEEN_UPLOADS_MS + " ms");
    }
    Serial.println((String)"Sending " + photocellVal);
    _adafruitIoFeed->save(photocellVal);
    _lastUploadedVal = photocellVal;
    _lastUploadTimestamp = millis();
  }

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
