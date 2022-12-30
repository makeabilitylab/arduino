#include <Arduino.h>

// https://github.com/adafruit/Adafruit_ZeroI2S/blob/master/Adafruit_ZeroI2S.h
// https://github.com/adafruit/Adafruit_ZeroI2S/blob/master/Adafruit_ZeroI2S.cpp
#include <Adafruit_ZeroI2S.h>
#include <math.h>

// See:
// - https://github.com/wjmb/FeatherM0_BEEP
// - https://github.com/adafruit/Adafruit_ZeroDMA/issues/12#issuecomment-530346409

// Use default pins in board variant
// (uint8_t FS_PIN, uint8_t SCK_PIN, uint8_t TX_PIN, uint8_t RX_PIN);
// Some info: https://github.com/espressif/arduino-esp32/blob/master/docs/source/api/i2s.rst
// The I2S bus contains at least three lines:
//
// * Word clock line
//   Officially "word select (WS)". Typically called "left-right clock (LRCLK)" or "frame sync (FS)".
//   0 = Left channel, 1 = Right channel
//   In some libs, this is called fsPin
//   The FS_PIN should be hooked up to Pin 0 (RX) on M0
//
// * The Bit Clock Line
//   Officially "continuous serial clock (SCK)". Typically written "bit clock (BCLK)".
//   In some libs, this is a called sckPin.
//   The SCK_PIN should be hooked up to Pin 1 (TX) on M0
//
// * Data line
//   Officially "serial data (SD)", but can be called SDATA, SDIN, SDOUT, DACDAT, ADCDAT, etc.
//   Can be two pins with one dedicated to input and one for output  
//   
//   
Adafruit_ZeroI2S i2s(0, 1, 9, 2);// = Adafruit_ZeroI2S(); RX, TX, 

int32_t audiosamples = 128;

void setup()
{
  while (!Serial) delay(10);
  

  Serial.println("I2S demo");

  // for(int i=0; i<BUFSIZE; i++){
  //     /* create a sine wave on the left channel */
  //       left[i] = sin( (2*PI / (BUFSIZE) ) * i) * VOLUME;

  //       /* create a cosine wave on the right channel */
  //       right[i] = cos( (2*PI / (BUFSIZE) ) * i) * VOLUME;
  // }

  /* begin I2S on the default pins. 24 bit depth at
   * 44100 samples per second
   */
  if (!i2s.begin(I2S_32_BIT, 44100)) {
    Serial.println("Failed to initialize I2S transmitter!");
    while (1);
  }
  i2s.enableRx(); 
  //pinMode(11, OUTPUT);
}

void loop()
{
  // This is a stereo Mic. But we'll only read from LEFT channel
  //digitalWrite(11, LOW);  

  int32_t left,right;
  int i;
  int sample=0;

  // Read a bunch of samples
  int32_t samples[audiosamples];
  
  for (int i = 0; i < audiosamples; i++) {
    i2s.read(&left, &right);
    if (left != -1) {
     
      //delay(1);  // Workaround delay to prevent oversizing the buffer
      sample = left;
      // convert to 18 bit signed
      sample >>= 14;
      samples[i] = abs(sample);
      Serial.println(samples[i]);
      //Serial.print(", ");
      //Serial.println(right);
    }else{
      delay(1);
      //Serial.println("Left == -1");
    }
  }

  //delay(100);
}