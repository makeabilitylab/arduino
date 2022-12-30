/**
 * This code is by chrisinsingapore2011 posted to the Adafruit forums here:
 * https://forums.adafruit.com/viewtopic.php?p=708786#p708786
 *
 * It is intended to address the I2S.read() hanging problem also described 
 * in that forum post—and which we also experienced—with the VUMeter code.
 *
 * This code appears to work more reliably than MicInputPlotter, which
 * is from https://learn.adafruit.com/adafruit-i2s-mems-microphone-breakout/arduino-wiring-and-test
 * and also posted in our GitHub here:
 * 
 * 
 * Note: To use an i2s peripheral like an i2s microphone, you must use a microcontroller
 * that have hardware I2S peripheral support such as the Cortex M-series chips like the Arduino Zero, Feather M0, 
 * or single-board computers like the Raspberry Pi. This code has been tested on an
 * Adafruit Feather M0 Express: https://www.adafruit.com/product/3403
 * with the Adafruit i2s MEMs Microphone Breakout for the SPH0645LM4H
 * https://www.adafruit.com/product/3421  
 */ 



#include <I2S.h>
#define INFINITY_NEGATIVE -99999
#define I2S_BUFFER_SIZE 512
#define I2S_BITS_PER_SAMPLE 32                    // 32-bits per I2S sample
uint8_t buffer[I2S_BUFFER_SIZE];                  // Allocate the buffer size
int *I2Svalues = (int *) buffer;                  // I2Svalues changes to integer type, rather than an unsigned int 8-bit type, same data, groups it differently
volatile boolean I2SAvailable = false;            // Shouldn't this be a boolean?
int I2Smax_2ms;                                   // Used to hold the maximum sound reading from each group of samples (i.e. each 2mS)
    
void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }                           // wait for serial port to connect. Needed for native USB port only
  I2S.onReceive(onI2SReceive);                    // Set the interrupt for when I2S data is available, to call the onI2SReceive sub
  int sampleRate = 31250;                         // 48,000,000 / sampleRate must be a multiple of 64
  if (!I2S.begin(I2S_PHILIPS_MODE, sampleRate, I2S_BITS_PER_SAMPLE)) { Serial.println("Failed to initialize I2S!"); while (1); } // do nothing
  I2S.read();                                     // One read call is required to start the interrupt handler
}

void onI2SReceive() { // This function will run at a frequency of (sampleRate / 64).  At 31.25khz, this is every 1962.5 microseconds so make sure any processing here takes less time than that
  I2Smax_2ms = INFINITY_NEGATIVE;                 // Reset the maximum counter to detect a new peak from this cycle
  if (I2SAvailable) { Serial.println("POTENTIAL OVERFLOW"); } // This indicates that the program is not keeping up with processing
  I2S.read(buffer, I2S_BUFFER_SIZE);              // Will actually read 256 bytes
  for (int i = 0; i < I2S_BITS_PER_SAMPLE; i++) { // Process the data
    if ((I2Svalues[i]!= 0) && (I2Svalues[i] != -1) ) {  // All 0 or -1 readings need to be discarded (every second reading depending on SEL
      I2Svalues[i] >>= 14;   // convert to 18 bit signed
      if (I2Smax_2ms < I2Svalues[i]) { I2Smax_2ms = I2Svalues[i]; } // Record the new maximum value       
    }
  }      
  Serial.println(I2Smax_2ms);
  I2SAvailable = true;
}

void loop() {
  if (I2SAvailable) {
    // Add a second round of processing to pick out a longer term maximum
    I2SAvailable = false;
  }
}