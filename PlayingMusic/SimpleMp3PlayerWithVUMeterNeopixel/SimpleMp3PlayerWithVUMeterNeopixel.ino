/**
 * WARNING: this sketch does not work correctly due to incompatibilities between NeoPixels 
 * and the VS1053 playback using interrupts. 
 *
 * Plays music and lights up a NeoPixel corresponding to sound amplitude.
 * 
 * Reads the SD card for all .mp3 files and starts playing them once the
 * next button (on pin 13) or previous button (on pin 12) is pressed
 *
 * If connected to Serial, open Serial Monitor and use the following commands:
 *  - Type 'n' for next song 
 *  - Type 'b' for previous song
 *  - Type 'p' to pause or unpause
 *
 * You can also connect buttons to pins 13 and 12 (with internal pullup configuration)
 * By default, pin 13 goes to the next song and pin 12 to the prev song
 *
 * --- Engineering Log ----
 * Jan 2, 2023: it looks like we cannot use NeoPixels and the VS1053 MP3 playback library at the same time
 * Because the VS1053 library requires interrupts for "background" playback and NeoPixels require that 
 * interrupts are disabled while writing to the strip. See: https://forums.adafruit.com/viewtopic.php?p=250721#p250721
 * They recommend using an LPD8806 strip instead. 
 *
 * I've also been investigating using the FastLED library instead of the official Adafruit_NeoPixel library; however,
 * even here they suggest using a 4-wire LED chipset like the APA102 or LPD8806 because these chipsets don't
 * have the very specific timing requirements that the NeoPixel (WS2812) strips require. See:
 * https://github.com/FastLED/FastLED/wiki/Interrupt-problems
 * 
 * Built on:
 *  - feather_player in File -> Examples -> Adafruit VS1053 Library -> feather_player
 *    or also found https://github.com/adafruit/Adafruit_VS1053_Library/blob/master/examples/feather_player/feather_player.ino
 * 
 *
 * Tested on:
 *  - Adafruit nRF52840: https://learn.adafruit.com/introducing-the-adafruit-nrf52840-feather
 * 
 * Maybe try other boards like:
 *  - Adafruit Feather MO: https://www.adafruit.com/product/3403
 *  - Adafruit Feather M4: https://www.adafruit.com/product/3857
 *
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */


#include <SPI.h>
#include <SD.h> // https://www.arduino.cc/reference/en/libraries/sd/
#include <FileUtils.hpp> // From MakeabilityLab_Arduino_Library
#include <Button.hpp> // From MakeabilityLab_Arduino_Library
#include <Adafruit_NeoPixel.h> // https://github.com/adafruit/Adafruit_NeoPixel

// https://github.com/adafruit/Adafruit_VS1053_Library/blob/master/Adafruit_VS1053.h
// https://github.com/adafruit/Adafruit_VS1053_Library/blob/master/Adafruit_VS1053.cpp
#include <Adafruit_VS1053.h> 
#include "VS1053_Pins.h"

const int NUM_NEOPIXELS = 30;       // Change this to match your strand length
const int NEOPIXEL_PIN_OUTPUT = 6;  // Change this to match your output pin

const unsigned long MAX_HUE_VALUE = 65535;  // Hue is a 16-bit number
const unsigned int MAX_BRIGHTNESS_VALUE = 255;
const unsigned int MAX_SATURATION_VALUE = 255;
Adafruit_NeoPixel _neopixelStrip = Adafruit_NeoPixel(NUM_NEOPIXELS, NEOPIXEL_PIN_OUTPUT,
                                                     NEO_GRB + NEO_KHZ800);

Adafruit_VS1053_FilePlayer _musicPlayer = 
  Adafruit_VS1053_FilePlayer(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);

File _fileRoot; // Object is SDLib.File in SD.h
String *_soundFiles = NULL;
int _curSoundFileIndex = 0;
int _numSoundFiles = 0;

// Next and prev buttons
Button _btnPrev = Button(12);
Button _btnNext = Button(13);

const int SOUND_LEVEL_LED_PIN = 11;
const int VOLUME_POT_PIN = A0;

// Max ADC on the NRF52840 is 12-bit (4096)
// But according to Adafruit docs, default values for ADC is 10-bit
// The default values for the ADC are 10-bit resolution (0..1023) with 
// a 3.6V reference voltage, meaning every digit returned from the 
// ADC = 3600mV/1024 = 3.515625mV.
const unsigned int MAX_ANALOG_IN = 1023; 
const unsigned int MAX_ANALOG_OUT = 255;
const int MIC_INPUT_PIN = A5;
const int MAX_MIC_LEVEL = MAX_ANALOG_IN;

const int MIC_SAMPLE_WINDOW_MS = 40; // Sample window width in ms (50 ms = 20Hz). 
unsigned long _startSampleTimeMs = -1;
unsigned int _signalMax = 0;
unsigned int _signalMin = MAX_ANALOG_IN;
unsigned int _numMicSamples = 0;
long _cumulativeMicLevel = 0;
long _totalMicSamples = 0;
long _startSamplingMicTimeMs = -1;

const String SONG_PATH = "/Dance/"; // can be just "/" if your songs are in root of SD card

// For debouncing
int _nextBtnStateSaved = HIGH;
int _prevBtnStateSaved = HIGH;

void setup() {
  Serial.begin(115200);

  //Wait for serial port to be opened, remove this line for 'standalone' operation
  //while (!Serial) { delay(1); }

  // Super fast analog sampling on nRF52840
  // See: https://devzone.nordicsemi.com/f/nordic-q-a/73466/faster-analog-sample-frequency-on-the-arduino-ide
  // nrf_saadc_acqtime_t acqTime = NRF_SAADC_ACQTIME_3US;
  // adcCurrentConfig.acq_time = acqTime;
  // analogUpdate();

  _neopixelStrip.begin();            // Initialize NeoPixel strip object (REQUIRED)
  _neopixelStrip.setBrightness(50);  // Set brightness to about 1/5 (max = 255)
  _neopixelStrip.rainbow(0);
  _neopixelStrip.show();             // Turn OFF all pixels ASAP
  delay(500);

  pinMode(MIC_INPUT_PIN, INPUT);
  pinMode(SOUND_LEVEL_LED_PIN, OUTPUT);

  // We must initialize the buttons
  _btnNext.begin();
  _btnPrev.begin();

  //if you're using Bluefruit or LoRa/RFM Feather, disable the radio module
  //pinMode(8, INPUT_PULLUP);

  delay(500);
  Serial.println("\n\nAdafruit VS1053 Feather Test");
  
  if (! _musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }

  Serial.println(F("VS1053 found"));
  
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  
  // list files
  FileUtils::printDirectory("/"); 
  _numSoundFiles = FileUtils::countMatchingFiles(SONG_PATH, ".mp3", false);

  Serial.print("The number of .mp3 files in " + SONG_PATH);
  Serial.println(_numSoundFiles);
  int numMatchingFiles = 0;
  _soundFiles = FileUtils::getFilePaths(SONG_PATH, ".mp3", numMatchingFiles); 
  
  // Set volume for left, right channels. lower numbers == louder volume!
  _musicPlayer.setVolume(5,5); // on headphones, i recommend more like 40, 40
  
#if defined(__AVR_ATmega32U4__) 
  // Timer interrupts are not suggested, better to use DREQ interrupt!
  // but we don't have them on the 32u4 feather...
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int
#else
  // If DREQ is on an interrupt pin we can do background
  // audio playing
  _musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
#endif

  // get random sound file index
  // if(_numSoundFiles > 0){
  //   randomSeed(analogRead(A0));
  //   _curSoundFileIndex = random(_numSoundFiles);
  //   Serial.print("The current sound file index: ");
  //   Serial.println(_curSoundFileIndex);
  //   Serial.print("The current sound filename: ");
  //   Serial.println(_soundFiles[_curSoundFileIndex]);
    
  //   Serial.println("Playing " + _soundFiles[_curSoundFileIndex] + " now at index " + _curSoundFileIndex);

  //   // https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/c_str/
  //   _musicPlayer.startPlayingFile(_soundFiles[_curSoundFileIndex].c_str());
  // }

  _startSampleTimeMs = millis();
}

void loop() {
  
  int volumePotVal = analogRead(VOLUME_POT_PIN);

  // read microphone and set LED
  //noInterrupts();
  if(_startSamplingMicTimeMs == -1){
    _startSamplingMicTimeMs = millis();
  }
  int micLevel = analogRead(MIC_INPUT_PIN);
  micLevel = analogRead(MIC_INPUT_PIN);
  _cumulativeMicLevel += micLevel;
  _numMicSamples++;
  _totalMicSamples++;
  
  if(micLevel > _signalMax){
    _signalMax = micLevel;
  }else if(micLevel < _signalMin){
    _signalMin = micLevel;
  }

  // TODO for more on sound
  // See: https://learn.adafruit.com/adafruit-i2s-mems-microphone-breakout/arduino-wiring-and-test
  if(millis() - _startSampleTimeMs > MIC_SAMPLE_WINDOW_MS){
    long peakToPeak = _signalMax - _signalMin;

    const int neoSaturation = MAX_SATURATION_VALUE;
    const int neoBrightness = 150;
    unsigned int numNeoPixelsToIlluminate = map(peakToPeak, 0, MAX_MIC_LEVEL, 0, NUM_NEOPIXELS);
    numNeoPixelsToIlluminate = constrain(numNeoPixelsToIlluminate, 0, NUM_NEOPIXELS);

    _neopixelStrip.clear();
    _neopixelStrip.setPixelColor(0, 255, 0, 0);
    _neopixelStrip.setPixelColor(1, 255, 0, 0);
    _neopixelStrip.setPixelColor(2, 0, 255, 0);
    _neopixelStrip.setPixelColor(3, 0, 0, 255);
    for(int pxl = 0; pxl < NUM_NEOPIXELS; pxl++){
      const int hueVal = map(pxl, 0, NUM_NEOPIXELS, 0, MAX_HUE_VALUE * .8);
      uint32_t rgbColor = _neopixelStrip.ColorHSV(0, neoSaturation, neoBrightness);

      // setPixelColor is an overloaded function that takes in the pixel index
      // with zero-based indexing and red, green, blue or simply pixel index
      // and a 32-bit type that merges red, green, blue values into a single number
      //_neopixelStrip.setPixelColor(0, 255, 0, 0);
      // if(pxl < numNeoPixelsToIlluminate){
      //   _neopixelStrip.setPixelColor(pxl, rgbColor);
      // }else{
      //   _neopixelStrip.setPixelColor(pxl, 0);
      // }
    }

    // This show() call disables interrupts. According to the code here:
    // https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.cpp
    // but that then impacts our playback library and causes it to stop working
    // Specifically, the docs state that we will lose 30 microseconds per RGB pixel
    _neopixelStrip.show();
    
    // TODO maybe change min here when music is not playing?
    int ledBrightnessVal = map(peakToPeak, 0, MAX_MIC_LEVEL, 0, MAX_ANALOG_OUT);
    long avgMicLevel = _cumulativeMicLevel / _numMicSamples;

    // Comment out this Serial.print block when not debugging
    Serial.print(_numMicSamples);
    Serial.print(", ");
    Serial.print(_signalMin);
    Serial.print(", ");
    Serial.print(_signalMax);
    Serial.print(", ");
    Serial.print(avgMicLevel);
    Serial.print(", ");
    Serial.print(peakToPeak);
    Serial.print(", ");
    Serial.print(volumePotVal);
    Serial.print(", ");
    Serial.println(ledBrightnessVal);
    analogWrite(SOUND_LEVEL_LED_PIN, ledBrightnessVal);

    _signalMax = 0;
    _signalMin = MAX_ANALOG_IN;
    _startSampleTimeMs = millis();
    _numMicSamples = 0;
    _cumulativeMicLevel = 0;
  }
  //interrupts();


  // TODO switch from linear mapping to logarithmic if using a linear pot
  uint8_t soundVolume = (uint8_t)map(volumePotVal, 0, MAX_ANALOG_IN, 0, 255);
  _musicPlayer.setVolume(soundVolume, soundVolume);

   // Check prev/next buttons
  _btnNext.read();
  _btnPrev.read();
  if(_btnNext.wasPressed()){
    playNextSound();
  }else if(_btnPrev.wasPressed()){
    playPrevSound();
  }

  if (Serial.available()) {
    char c = Serial.read();
    
    // if we get an 's' on the serial console, stop!
    if (c == 's') {
      _musicPlayer.stopPlaying();

      // print out sample rate
      long samplingTime = millis() - _startSamplingMicTimeMs;
      Serial.print("We captured a total of ");
      Serial.print(_totalMicSamples);
      Serial.print(" mic samples in ");
      Serial.print(samplingTime / 1000.0);
      Serial.println(" secs");
      _startSamplingMicTimeMs = -1;
      _totalMicSamples = 0;
    }
    
    // if we get an 'p' on the serial console, pause/unpause!
    if (c == 'p') {
      if (! _musicPlayer.paused()) {
        Serial.println("Paused");
        _musicPlayer.pausePlaying(true);
      } else { 
        Serial.println("Resumed");
        _musicPlayer.pausePlaying(false);
      }
    }

    if (c == 'n'){
      playNextSound();
    }

    if (c == 'b'){
      playPrevSound();
    }
  }
  
  //delay(100);
}

void playNextSound(){
  Serial.println();
  Serial.println("Stop playing current song...");
  Serial.println("Stopping " + _soundFiles[_curSoundFileIndex] + " at index " + (String)_curSoundFileIndex);

  _curSoundFileIndex++;
  if(_curSoundFileIndex >= _numSoundFiles){
    _curSoundFileIndex = 0;
  }
  
  startPlayingSound(_curSoundFileIndex);
}

void playPrevSound(){
  Serial.println();
  Serial.println("Stop playing current song...");
  Serial.println("Stopping " + _soundFiles[_curSoundFileIndex] + " at index " + (String)_curSoundFileIndex);

  _curSoundFileIndex--;
  if(_curSoundFileIndex < 0){
    _curSoundFileIndex = _numSoundFiles - 1;
  }

  startPlayingSound(_curSoundFileIndex);
}

void startPlayingSound(int curSoundFileIndex){
  // We *must* call noInterrupts() here before trying to stop the music
  // If not, we will intermittently run into a freezing problem with the startPlayingFile
  // call. It happens consistently but infrequently enough to be hard to test and track down.
  //
  // I arrived at stopping interrupts by sleuthing the VS_1053 source code
  // https://github.com/adafruit/Adafruit_VS1053_Library/blob/master/Adafruit_VS1053.cpp
  noInterrupts();
  _musicPlayer.stopPlaying();
  delay(50);
  
  if(!_musicPlayer.playingMusic){ // using this rather than stopped()
    Serial.println("The music player has stopped");
  }else{
    Serial.println("The music player has NOT stopped");
  }

  Serial.println("The next song to play is " + _soundFiles[curSoundFileIndex] + " at index " + curSoundFileIndex);
  Serial.println("Calling _musicPlayer.startPlayingFile...");

  // Start playing next song
  boolean startPlaying = _musicPlayer.startPlayingFile(_soundFiles[curSoundFileIndex].c_str());
  Serial.print("startPlayingFile returned: ");
  Serial.println(startPlaying);

  // Sometimes we don't start right away
  while(!startPlaying){
    Serial.println("Waiting for song to start...");
    startPlaying = _musicPlayer.startPlayingFile(_soundFiles[curSoundFileIndex].c_str());
    if(!startPlaying){
      delay(100);
    }
  }
  Serial.println();
  Serial.println("Playing " + _soundFiles[curSoundFileIndex] + " at index " + (String)curSoundFileIndex);
}
