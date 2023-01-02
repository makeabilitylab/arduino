/**
 * WARNING: this sketch doesn't work due to incompatibilities between the i2s mic library
 * and the mp3 player library.
 *
 * Plays music and lights up an LED corresponding to amplitude.
 * This is my attempt to use an i2s mic with the mp3 player... unfortunately, it is not working
 *  
 * 
 * Reads the SD card for all .mp3 files and starts playing them
 * If connected to Serial, open Serial Monitor and use the following commands:
 *  - Type 'n' for next song 
 *  - Type 'b' for previous song
 *  - Type 'p' to pause or unpause
 *
 * You can also connect buttons to pins 13 and 12 (with internal pullup configuration)
 * By default, pin 13 goes to the next song and pin 12 to the prev song
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

// The Arduino i2s lib is not supported by Adafruit boards
// see: https://github.com/adafruit/Adafruit_ZeroDMA/issues/12#issuecomment-527993959
// So need to use https://github.com/adafruit/Adafruit_ZeroI2S instead
#include <I2S.h> 
#include <SPI.h>
#include <SD.h> // https://www.arduino.cc/reference/en/libraries/sd/
#include <FileUtils.hpp> // From MakeabilityLab_Arduino_Library

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

const int NEXT_BUTTON_PIN = 13;
const int PREV_BUTTON_PIN = 12;
const int SOUND_LEVEL_LED_PIN = 11;
const int VOLUME_POT_PIN = A0;

// Max ADC on the NRF52840 is 12-bit (4096)
// But according to Adafruit docs, default values for ADC is 10-bit
// The default values for the ADC are 10-bit resolution (0...1023) with 
// a 3.6V reference voltage, meaning every digit returned from the 
// ADC = 3600mV/1024 = 3.515625mV.
// I just checked that 1023 is the default max value on the Feather Express M0 too.
const unsigned int MAX_ANALOG_IN = 1023; 
const unsigned int MAX_ANALOG_OUT = 255;

const String SONG_PATH = "/Dance/"; // can be just "/" if your songs are in root of SD card

void setup() {
  Serial.begin(115200);

  pinMode(SOUND_LEVEL_LED_PIN, OUTPUT);
  pinMode(NEXT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(PREV_BUTTON_PIN, INPUT_PULLUP);

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
}

void loop() {
  

  // read buttons and volume
  int nextBtnState = digitalRead(NEXT_BUTTON_PIN);
  int prevBtnState = digitalRead(PREV_BUTTON_PIN);
  int volumePotVal = analogRead(VOLUME_POT_PIN);

  // TODO switch from linear mapping to logarithmic if using a linear pot
  uint8_t soundVolume = (uint8_t)map(volumePotVal, 0, MAX_ANALOG_IN, 255, 0); // 0 is loudest sound volume
  _musicPlayer.setVolume(soundVolume, soundVolume);

  if(nextBtnState == LOW){
    playNextSound();
  }else if(prevBtnState == LOW){
    playPrevSound();
  }

  if (Serial.available()) {
    char c = Serial.read();
    
    // if we get an 's' on the serial console, stop!
    if (c == 's') {
      _musicPlayer.stopPlaying();
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
