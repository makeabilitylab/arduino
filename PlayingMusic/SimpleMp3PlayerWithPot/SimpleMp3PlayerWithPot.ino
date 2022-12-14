/**
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

// https://github.com/adafruit/Adafruit_VS1053_Library/blob/master/Adafruit_VS1053.h
// https://github.com/adafruit/Adafruit_VS1053_Library/blob/master/Adafruit_VS1053.cpp
#include <Adafruit_VS1053.h> 

// Specifically for use with the Adafruit Feather, the pins are pre-set here!

// These are the pins used
#define VS1053_RESET   -1     // VS1053 reset pin (not used!)

// Feather ESP8266
#if defined(ESP8266)
  #define VS1053_CS      16     // VS1053 chip select pin (output)
  #define VS1053_DCS     15     // VS1053 Data/command select pin (output)
  #define CARDCS          2     // Card chip select pin
  #define VS1053_DREQ     0     // VS1053 Data request, ideally an Interrupt pin

// Feather ESP32
#elif defined(ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
  #define VS1053_CS      32     // VS1053 chip select pin (output)
  #define VS1053_DCS     33     // VS1053 Data/command select pin (output)
  #define CARDCS         14     // Card chip select pin
  #define VS1053_DREQ    15     // VS1053 Data request, ideally an Interrupt pin

// Feather Teensy3
#elif defined(TEENSYDUINO)
  #define VS1053_CS       3     // VS1053 chip select pin (output)
  #define VS1053_DCS     10     // VS1053 Data/command select pin (output)
  #define CARDCS          8     // Card chip select pin
  #define VS1053_DREQ     4     // VS1053 Data request, ideally an Interrupt pin

// WICED feather
#elif defined(ARDUINO_STM32_FEATHER)
  #define VS1053_CS       PC7     // VS1053 chip select pin (output)
  #define VS1053_DCS      PB4     // VS1053 Data/command select pin (output)
  #define CARDCS          PC5     // Card chip select pin
  #define VS1053_DREQ     PA15    // VS1053 Data request, ideally an Interrupt pin

#elif defined(ARDUINO_NRF52832_FEATHER )
  #define VS1053_CS       30     // VS1053 chip select pin (output)
  #define VS1053_DCS      11     // VS1053 Data/command select pin (output)
  #define CARDCS          27     // Card chip select pin
  #define VS1053_DREQ     31     // VS1053 Data request, ideally an Interrupt pin

// Feather M4, M0, 328, ESP32S2, nRF52840 or 32u4
#else
  #define VS1053_CS       6     // VS1053 chip select pin (output)
  #define VS1053_DCS     10     // VS1053 Data/command select pin (output)
  #define CARDCS          5     // Card chip select pin
  // DREQ should be an Int pin *if possible* (not possible on 32u4)
  #define VS1053_DREQ     9     // VS1053 Data request, ideally an Interrupt pin
#endif

Adafruit_VS1053_FilePlayer _musicPlayer = 
  Adafruit_VS1053_FilePlayer(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);

File _fileRoot; // Object is SDLib.File in SD.h
String *_soundFiles = NULL;
int _curSoundFileIndex = 0;
int _numSoundFiles = 0;

const int NEXT_BUTTON_PIN = 13;
const int PREV_BUTTON_PIN = 12;
const int VOLUME_POT_PIN = A0;

// Max ADC on the NRF52840 is 12-bit (4096)
// See: https://learn.adafruit.com/introducing-the-adafruit-nrf52840-feather/pinouts#analog-inputs-3014279
const int MAX_ADC_VAL = 4096; 

void setup() {
  Serial.begin(115200);

  pinMode(NEXT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(PREV_BUTTON_PIN, INPUT_PULLUP);

  // if you're using Bluefruit or LoRa/RFM Feather, disable the radio module
  // pinMode(8, INPUT_PULLUP);
  
  // Wait for serial port to be opened, remove this line for 'standalone' operation
  // while (!Serial) { delay(1); }

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
  _fileRoot = SD.open("/");
  printDirectory(_fileRoot, 0); // FILE_READ
  _fileRoot.rewindDirectory(); // goes back to first file in dir

  _numSoundFiles = countFilesInDir(_fileRoot, ".mp3");
  Serial.print("The number of .mp3 files on the SD card: ");
  Serial.println(_numSoundFiles);
  _soundFiles = getSoundFiles(_fileRoot, ".mp3"); 
  
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

// Count the files in the current directory with the given extension
int countFilesInDir(File dir, String fileExt){
  // Force lower case for fileExt, which is done "in place"
  // https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/tolowercase/
  fileExt.toLowerCase(); 

  int cntMatchingFiles = 0;
  if(dir && dir.isDirectory()){
    // https://www.arduino.cc/reference/en/libraries/sd/opennextfile/
    File fileEntry =  dir.openNextFile();
    while(fileEntry){
      if(fileEntry && !fileEntry.isDirectory()){
        // https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/endswith/
        String entryFilename = String(fileEntry.name());
        entryFilename.toLowerCase();
        if(entryFilename.endsWith(fileExt)){
          cntMatchingFiles++;
          Serial.println(entryFilename + " ends with " + fileExt);
        }else{
          Serial.println(entryFilename + " does NOT end with " + fileExt);
        }
      }
      fileEntry =  dir.openNextFile();
    }
  }else{
    if(dir){
      Serial.println("The file entry " + (String)dir.name() + " is not a directory");
    }else{
      Serial.print("The dir is blank or null: ");
      Serial.println(dir);
    }
  }
  dir.rewindDirectory();
  return cntMatchingFiles;
}

//WARNING: allocates memory for String array that would need to be deleted (if no longer used)
//Gets the sound files in the current dir
String* getSoundFiles(File dir, String fileExt){
  // Force lower case for fileExt, which is done "in place"
  // https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/tolowercase/
  fileExt.toLowerCase(); 
 
  int numSoundFiles = countFilesInDir(dir, fileExt);
  String *soundFiles = new String[numSoundFiles];
  int soundFileIndex = 0;
  if(dir && dir.isDirectory()){
    File fileEntry =  dir.openNextFile();
    while(fileEntry){
      if(fileEntry && !fileEntry.isDirectory()){
        // https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/endswith/
        String entryFilename = String(fileEntry.name());
        entryFilename.toLowerCase();
        if(entryFilename.endsWith(fileExt)){
          // Serial.println("Adding " + entryFilename + " at index " + (String)soundFileIndex);
          soundFiles[soundFileIndex++] = entryFilename;
        }
      }
      fileEntry =  dir.openNextFile();
    }
  }
  dir.rewindDirectory();
  return soundFiles;
}

void loop() {
  
  // File is playing in the background
  if (_musicPlayer.stopped()) {
    Serial.println("Music player stopped");
  }else{
    Serial.print(".");
  }

  int nextBtnState = digitalRead(NEXT_BUTTON_PIN);
  int prevBtnState = digitalRead(PREV_BUTTON_PIN);
  int volumePotVal = analogRead(VOLUME_POT_PIN);

  // TODO switch from linear mapping to logarithmic if using a linear pot
  uint8_t soundVolume = (uint8_t)map(volumePotVal, 0, MAX_ADC_VAL, 0, 255);
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
  delay(100);
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

  // I also inserted this explicit close to get the music player to try and fix
  // the freezing problem. Not sure if it actually helps.
  // See: "VS1053 frequently crashes when changing audio files" 
  // https://forums.adafruit.com/viewtopic.php?p=268476#p268476
  _musicPlayer.currentTrack.close();
  delay(50);
  
  if(!_musicPlayer.playingMusic){ // using this rather than stopped()
    Serial.println("The music player has stopped");
  }else{
    Serial.println("The music player has NOT stopped");
  }
 
  //interrupts();
  //Serial.println("Interrupts enabled");

  // GitHub Issue post suggests that softReset() might help freezing problem
  // https://github.com/adafruit/Adafruit_VS1053_Library/issues/4
  // Unfortunately, that does not seem to work...
  // Serial.println("Performing a softReset()");
  // _musicPlayer.softReset();
  // delay(50);

  // How about a hard reset? This also does not appear to fix the problem
  // So commenting out for now...
  // Serial.println("Performing a reset()");
  // _musicPlayer.reset();
  // delay(50);

  Serial.println("The next song to play is " + _soundFiles[curSoundFileIndex] + " at index " + curSoundFileIndex);
  Serial.println("Calling _musicPlayer.startPlayingFile...");
  delay(50);

  // Sometimes we crash right here!
  boolean startPlaying = _musicPlayer.startPlayingFile(_soundFiles[curSoundFileIndex].c_str());
  delay(50);

  Serial.print("startPlayingFile returned: ");
  Serial.println(startPlaying);
  while(!startPlaying){
    Serial.println("Waiting for song to start...");
    startPlaying = _musicPlayer.startPlayingFile(_soundFiles[curSoundFileIndex].c_str());
    delay(200);
  }
  Serial.println();
  Serial.println("Playing " + _soundFiles[curSoundFileIndex] + " at index " + (String)curSoundFileIndex);
}

// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
