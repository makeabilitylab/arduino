/**
 * Plays music and lights up an LED corresponding to amplitude.
 * 
 *  * Reads the SD card for all .mp3 files and starts playing them
 * If connected to Serial, open Serial Monitor and use the following commands:
 *  - Type 'n' for next song 
 *  - Type 'b' for previous song
 *  - Type 'p' to pause or unpause
 *
 * You can also connect buttons to pins 13 and 12 (with internal pullup configuration)
 * By default, pin 13 goes to the next song and pin 12 to the prev song
 *
 * --- Engineering Log ----
 * The microphone does not seem to work as expected when music is playing
 * not sure if it's noise or the interrupts or what. I've tried multiple things
 * to investigate. To test, "stop" the song by sending 's' or 'p' in Serial Monitor
 * and then make noise. You should see the mic respond accordingly. But then
 * when you actually play music, the mic levels are messed up... 
 * 
 * Update (Dec 30): I determined that this was due to noise, likely due to unstable
 * power on the 3.3V, which would fluctuate when playing mp3s (likely due to featherwing draw)
 * I'm also going to ry a 100uF capacitor and to try a MAX9814 amp vs. the MAX4466
 * that I've been using.
 *
 * Update 2 (Dec 30): I've now tested with a 100uF capacitor, which *appears* to help. I've
 * also tested with both a MAX9814 and MAX4466. It appears that the MAX9814 works better but
 * both are reasonable.
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

// https://github.com/adafruit/Adafruit_VS1053_Library/blob/master/Adafruit_VS1053.h
// https://github.com/adafruit/Adafruit_VS1053_Library/blob/master/Adafruit_VS1053.cpp
#include <Adafruit_VS1053.h> 
#include "VS1053_Pins.h"

Adafruit_VS1053_FilePlayer _musicPlayer = 
  Adafruit_VS1053_FilePlayer(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);

File _fileRoot; // Object is SDLib.File in SD.h
String *_soundFiles = NULL;
int _curSoundFileIndex = 0;
int _numSoundFiles = 0;

const int VOLUME_POT_PIN = A0;

// Next and prev buttons
Button _btnPrev = Button(12);
Button _btnNext = Button(13);

// Max ADC on the NRF52840 is 12-bit (4096)
// But according to Adafruit docs, default values for ADC is 10-bit
// The default values for the ADC are 10-bit resolution (0..1023) with 
// a 3.6V reference voltage, meaning every digit returned from the 
// ADC = 3600mV/1024 = 3.515625mV.
const unsigned int MAX_ANALOG_IN = 1023; 
const unsigned int MAX_ANALOG_OUT = 255;
const int MIC_INPUT_PIN = A5;
const int MAX_MIC_LEVEL = MAX_ANALOG_IN;

// Microphone variables
const int SOUND_LEVEL_LED_PIN = 11;
const int MIC_SAMPLE_WINDOW_MS = 40; // Sample window width in ms (50 ms = 20Hz). 
unsigned long _startSampleTimeMs = -1;
unsigned int _signalMax = 0;
unsigned int _signalMin = MAX_ANALOG_IN;
unsigned int _numMicSamples = 0;
long _cumulativeMicLevel = 0;
long _totalMicSamples = 0;
long _startSamplingMicTimeMs = -1;

const String SONG_PATH = "/Dance/"; // can be just "/" if your songs are in root of SD card

void setup() {
  Serial.begin(115200);

  // Super fast analog sampling on nRF52840
  // See: https://devzone.nordicsemi.com/f/nordic-q-a/73466/faster-analog-sample-frequency-on-the-arduino-ide
  // nrf_saadc_acqtime_t acqTime = NRF_SAADC_ACQTIME_3US;
  // adcCurrentConfig.acq_time = acqTime;
  // analogUpdate();

  pinMode(MIC_INPUT_PIN, INPUT);
  pinMode(SOUND_LEVEL_LED_PIN, OUTPUT);

  // We must initialize the buttons
  _btnNext.begin();
  _btnPrev.begin();

  //if you're using Bluefruit or LoRa/RFM Feather, disable the radio module
  //pinMode(8, INPUT_PULLUP);
  
  //Wait for serial port to be opened, remove this line for 'standalone' operation
  while (!Serial) { delay(1); }

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
  if(_numSoundFiles > 0){
    randomSeed(analogRead(A0));
    _curSoundFileIndex = random(_numSoundFiles);
    Serial.print("The current sound file index: ");
    Serial.println(_curSoundFileIndex);
    Serial.print("The current sound filename: ");
    Serial.println(_soundFiles[_curSoundFileIndex]);
    
    Serial.println("Playing " + _soundFiles[_curSoundFileIndex] + " now at index " + _curSoundFileIndex);

    // https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/c_str/
    _musicPlayer.startPlayingFile(_soundFiles[_curSoundFileIndex].c_str());
  }

  _startSampleTimeMs = millis();
}

void loop() {
  
  int volumePotVal = analogRead(VOLUME_POT_PIN);
  //volumePotVal = 20;
  // File is playing in the background
  // if (!_musicPlayer.stopped()) {
  //   Serial.print(".");
  // }

  // read microphone and set LED
  noInterrupts();
  if(_startSamplingMicTimeMs == -1){
    _startSamplingMicTimeMs = millis();
  }
  int micLevel = analogRead(MIC_INPUT_PIN);
  micLevel = analogRead(MIC_INPUT_PIN);
  _cumulativeMicLevel += micLevel;
  _numMicSamples++;
  _totalMicSamples++;
  interrupts();
  //Serial.println(micLevel);

  // Serial.print(_numMicSamples);
  // Serial.print(", ");
  // Serial.println(micLevel);

  //Serial.println(micLevel);
  if(micLevel > _signalMax){
    _signalMax = micLevel;
  }else if(micLevel < _signalMin){
    _signalMin = micLevel;
  }

  // For more on some basic Arduino-based sound processing, see the example code here:
  // See: https://learn.adafruit.com/adafruit-i2s-mems-microphone-breakout/arduino-wiring-and-test
  if(millis() - _startSampleTimeMs > MIC_SAMPLE_WINDOW_MS){
    long peakToPeak = _signalMax - _signalMin;

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
