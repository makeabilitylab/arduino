/**
 * A Flappy Bird game for the OLED display. You can either use a button
 * to flap or serial input. For serial input, current reads two floats off serial (x, y), 
 * which are normalized to be between [0, 1] inclusive, and only uses the y position to 
 * set bird position. 
 * 
 * Works with the p5.js application:
 * https://makeabilitylab.github.io/p5js/WebSerial/ml5js/NoseTracker/
 *  
 * This code requires Shape.hpp from the MakeabilityLab_Arduino_Library
 * 
 * To install and use these files:
 * 1. Get it from here: https://github.com/makeabilitylab/arduino/tree/master/MakeabilityLab_Arduino_Library
 * 2. Move the entire folder to the Arduino libraries folder on your computer.
 *   - On my Windows box, this is C:\Users\jonfr\Documents\Arduino\libraries
 *   - On Mac, it's: /Users/jonf/Documents/Arduino/libraries
 * 3. Then include the relevant libraries via #include <libraryname.h> or <libraryname.hpp>
 *
 * Adafruit Gfx Library:
 * https://learn.adafruit.com/adafruit-gfx-graphics-library/overview 
 *
 * Adafruit OLED tutorials:
 * https://learn.adafruit.com/monochrome-oled-breakouts
 * 
 * Code based on:
 * https://makeabilitylab.github.io/p5js/Games/FlappyBird
 * https://makeabilitylab.github.io/p5js/Games/FlappyBird2
 *  
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

#include <Wire.h>
#include <Shape.hpp>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED _display width, in pixels
#define SCREEN_HEIGHT 64 // OLED _display height, in pixels

// Declaration for an SSD1306 _display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char STR_LOADSCREEN_CREATOR[] = "Makeability Lab";
const char STR_LOADSCREEN_APP_NAME_LINE1[] = "Flappy";
const char STR_LOADSCREEN_APP_NAME_LINE2[] = "Bird!";
const char STR_PRESS_FLAP_TO_PLAY[] = "Press flap to play";
const char STR_GAME_OVER[] = "Game Over!";

// Define I/O pins
const int FLAP_BUTTON_INPUT_PIN = 4;
const int SELECTION_BUTTON_INPUT_PIN = 5;
const int TONE_OUTPUT_PIN = 8;
const int VIBROMOTOR_OUTPUT_PIN = 9;

// for tracking fps
unsigned long _frameCount = 0;
float _fps = 0;
unsigned long _fpsStartTimeStamp = 0;

// status bar
const boolean _drawFrameCount = false; // change to show/hide frame count
const int LOAD_SCREEN_SHOW_MS = 750;

class Bird : public Rectangle {
  public:
    Bird(int x, int y, int width, int height) : Rectangle(x, y, width, height)
    {
    }
};

class Pipe : public Rectangle {
  protected:
    bool _hasPassedBird = false;

  public:
    Pipe(int x, int y, int width, int height) : Rectangle(x, y, width, height)
    {
    }

    bool getHasPassedBird() {
      return _hasPassedBird;
    }

    bool setHasPassedBird(bool hasPassedBird) {
      _hasPassedBird = hasPassedBird;
    }
};

const int BIRD_HEIGHT = 5;
const int BIRD_WIDTH = 10;
const int NUM_PIPES = 3;

const int MIN_PIPE_WIDTH = 8;
const int MAX_PIPE_WIDTH = 18; // in pixels
const int MIN_PIPE_X_SPACING_DISTANCE = BIRD_WIDTH * 3; // in pixels
const int MAX_PIPE_X_SPACING_DISTANCE = 100; // in pixels
const int MIN_PIPE_Y_SPACE = BIRD_HEIGHT * 3;
const int MAX_PIPE_Y_SPACE = SCREEN_HEIGHT - BIRD_HEIGHT * 2;

int _pipeSpeed = 2;
int _gravity = 1; // can't apply gravity every frame, apply every X time
int _points = 0;
unsigned long _gameOverTimestamp = 0;
bool _isInvincible = false;

const int IGNORE_INPUT_AFTER_GAME_OVER_MS = 500; //ignores input for 500ms after game over

// Initialize top pipe and bottom pipe arrays. The location/sizes don't matter
// at this point as we'll set them in setup()
Pipe _topPipes[NUM_PIPES] = { Pipe(0, 0, 0, 0),
                              Pipe(0, 0, 0, 0),
                              Pipe(0, 0, 0, 0)
                            };

Pipe _bottomPipes[NUM_PIPES] = { Pipe(0, 0, 0, 0),
                                 Pipe(0, 0, 0, 0),
                                 Pipe(0, 0, 0, 0)
                               };

Bird _bird(5, SCREEN_HEIGHT / 2, BIRD_WIDTH, BIRD_HEIGHT);

enum GameState {
  NEW_GAME,
  PLAYING,
  GAME_OVER,
};

enum FlapInputMode {
  FLAP_BUTTON,
  FLAP_SERIAL,
  NUM_INPUT_MODES
};

FlapInputMode _flapInputMode = FLAP_SERIAL;
int _lastInputModeButtonVal = HIGH;
GameState _gameState = NEW_GAME;

// This is necessary for the game to work on the ESP32
// See: 
//  - https://github.com/espressif/arduino-esp32/issues/1734
//  - https://github.com/Bodmer/TFT_eSPI/issues/189
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

float _serialX = 0;
float _serialY = 0;

const int BAUD_RATE = 115200;

void setup() {
  Serial.begin(BAUD_RATE);

  pinMode(FLAP_BUTTON_INPUT_PIN, INPUT_PULLUP);
  pinMode(SELECTION_BUTTON_INPUT_PIN, INPUT_PULLUP);

  // SSD1306_SWITCHCAPVCC = generate _display voltage from 3.3V internally
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // if analog input pin 5 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(A5));

  // Show load screen
  showLoadScreen();

  // Setup pipes
  initializeGameEntities();

  _fpsStartTimeStamp = millis();
}

void loop() {

  _display.clearDisplay();

  checkAndParseSerialData();

  drawStatusBar();
  if (_gameState == NEW_GAME || _gameState == GAME_OVER) {
    nonGamePlayLoop();
  } else if (_gameState == PLAYING) {
    gamePlayLoop();
  }
  calcFrameRate();

  // Draw the display buffer to the screen
  _display.display();
}

void checkAndParseSerialData(){
  // Check to see if there is any incoming serial data
  if(Serial.available() > 0){
    // If we're here, then serial data has been received
    // Read data off the serial port until we get to the endline delimiter ('\n')
    // Store all of this data into a string
    String rcvdSerialData = Serial.readStringUntil('\n'); 

    // Parse out the comma separated string
    int indexOfComma = rcvdSerialData.indexOf(',');
    
    if(indexOfComma != -1){
      String strXLocation = rcvdSerialData.substring(0, indexOfComma);
      _serialX = strXLocation.toFloat();

      String strYLocation = rcvdSerialData.substring(indexOfComma + 1, rcvdSerialData.length());
      _serialY = strYLocation.toFloat();
    } 
    
    // Echo the data back on serial (for debugging purposes)
    //Serial.print("# Arduino Received: '");
    //Serial.print(rcvdSerialData);
    //Serial.println("'");
  }
}

void drawInputModeMenu(int yText){
  int16_t x1, y1;
  uint16_t w, h;
  const String strInputMode = "Input Mode:";
  
  _display.setTextSize(1);
  _display.getTextBounds(strInputMode, 0, 0, &x1, &y1, &w, &h);
  int xText = _display.width() / 2 - w / 2;
  _display.setCursor(xText, yText);
  _display.println("INPUT MODE:");
  yText += h + 3;

  int inputModeButtonVal = digitalRead(SELECTION_BUTTON_INPUT_PIN);
  if(inputModeButtonVal == LOW && _lastInputModeButtonVal != inputModeButtonVal){
    _flapInputMode = (FlapInputMode)((int)_flapInputMode + 1);

    if(_flapInputMode >= NUM_INPUT_MODES){
      _flapInputMode = FLAP_BUTTON;
    }
    delay(50);
  }
  
  const int radius = 3;
  int xCircle = xText + radius + 1;
  int yCircle = yText + radius;
  if(_flapInputMode == FLAP_BUTTON){
    _display.fillCircle(xCircle, yCircle, radius, SSD1306_WHITE);
  }else{
    _display.drawCircle(xCircle, yCircle, radius, SSD1306_WHITE);
  }

  xText = xCircle + radius + 3;
  _display.setCursor(xText, yText);
  _display.print("Button"); 
  
  yText += h + 2;
  yCircle = yText + radius;
  if(_flapInputMode == FLAP_SERIAL){
    _display.fillCircle(xCircle, yCircle, radius, SSD1306_WHITE);
  }else{
    _display.drawCircle(xCircle, yCircle, radius, SSD1306_WHITE);
  }

  _display.setCursor(xText, yText);
  _display.print("Serial"); 

  _lastInputModeButtonVal = inputModeButtonVal;
}

void nonGamePlayLoop() {
  for (int i = 0; i < NUM_PIPES; i++) {
    _topPipes[i].draw(_display);
    _bottomPipes[i].draw(_display);
  }

  int16_t x1, y1;
  uint16_t w, h;
  int flapButtonVal = digitalRead(FLAP_BUTTON_INPUT_PIN);
  if (_gameState == NEW_GAME) {

    int yText = 11;
    _display.getTextBounds(STR_PRESS_FLAP_TO_PLAY, 0, 0, &x1, &y1, &w, &h);
    _display.setCursor(_display.width() / 2 - w / 2, yText);
    _display.print(STR_PRESS_FLAP_TO_PLAY);
    yText += h + 4;
    
    drawInputModeMenu(yText);

    if (flapButtonVal == LOW) {
      _gameState = PLAYING;
    }
  } else if (_gameState == GAME_OVER) {
    _display.setTextSize(2);
    _display.getTextBounds(STR_GAME_OVER, 0, 0, &x1, &y1, &w, &h);
    int yText = 0;
    _display.setCursor(_display.width() / 2 - w / 2, yText);
    _display.print(STR_GAME_OVER);

    yText = yText + h + 5;
    _display.setTextSize(1);
    _display.getTextBounds(STR_PRESS_FLAP_TO_PLAY, 0, 0, &x1, &y1, &w, &h);
    _display.setCursor(_display.width() / 2 - w / 2, yText);
    _display.print(STR_PRESS_FLAP_TO_PLAY);
    yText += h + 4;
    drawInputModeMenu(yText);

    // We ignore input a bit after game over so that user can see end game screen
    // and not accidentally start a new game
    if (flapButtonVal == LOW && millis() - _gameOverTimestamp >= IGNORE_INPUT_AFTER_GAME_OVER_MS) {
      // if the current state is game over, need to reset
      initializeGameEntities();
      _gameState = PLAYING;
    }
  }

  _bird.draw(_display);
}

void initializeGameEntities() {
  _points = 0;

  _bird.setY(_display.height() / 2 - _bird.getHeight() / 2);
  _bird.setDrawFill(true);

  const int minStartXPipeLocation = _display.width() / 2;
  int lastPipeX = minStartXPipeLocation;
  for (int i = 0; i < NUM_PIPES; i++) {

    int pipeX = lastPipeX + random(MIN_PIPE_X_SPACING_DISTANCE, MAX_PIPE_X_SPACING_DISTANCE);
    int pipeWidth = random(MIN_PIPE_WIDTH, MAX_PIPE_WIDTH);

    int yGapBetweenPipes = random(MIN_PIPE_Y_SPACE, MAX_PIPE_Y_SPACE);

    int topPipeY = 0;
    int topPipeHeight = random(0, SCREEN_HEIGHT - yGapBetweenPipes);

    int bottomPipeY = topPipeHeight + yGapBetweenPipes;
    int bottomPipeHeight = SCREEN_HEIGHT - bottomPipeY;

    _topPipes[i].setLocation(pipeX, topPipeY);
    _topPipes[i].setDimensions(pipeWidth, topPipeHeight);
    _topPipes[i].setDrawFill(false);

    _bottomPipes[i].setLocation(pipeX, bottomPipeY);
    _bottomPipes[i].setDimensions(pipeWidth, bottomPipeHeight);
    _topPipes[i].setDrawFill(false);

    lastPipeX = _topPipes[i].getRight();
  }
}

void gamePlayLoop() {
  
  if(_flapInputMode == FLAP_SERIAL){
    int y = _serialY * (_display.height() - _bird.getHeight());
    //Serial.println((String)"#" + y);
    Serial.print(_serialY);
    Serial.print(", ");
    Serial.println(y);
    _bird.setY(y);
  }else{
    int flapButtonVal = digitalRead(FLAP_BUTTON_INPUT_PIN);
    _bird.setY(_bird.getY() + _gravity);
    
    if (flapButtonVal == LOW) {
      _bird.setY(_bird.getY() - 3);
    }
  }

  _bird.forceInside(0, 0, _display.width(), _display.height());

  // xMaxRight tracks the furthest right pixel of the furthest right pipe
  // which we will use to reposition pipes that go off the left part of screen
  int xMaxRight = 0;

  // Iterate through pipes and check for collisions and scoring
  for (int i = 0; i < NUM_PIPES; i++) {

    _topPipes[i].setX(_topPipes[i].getX() - _pipeSpeed);
    _bottomPipes[i].setX(_bottomPipes[i].getX() - _pipeSpeed);

    _topPipes[i].draw(_display);
    _bottomPipes[i].draw(_display);

    // Check if the bird passed by the pipe
    if (_topPipes[i].getRight() < _bird.getLeft()) {

      // If we're here, the bird has passed the pipe. Check to see
      // if we've marked it as passed yet. If not, then increment the score!
      if (_topPipes[i].getHasPassedBird() == false) {
        _points++;
        _topPipes[i].setHasPassedBird(true);
        _bottomPipes[i].setHasPassedBird(true);
      }
    }

    // xMaxRight is used to track future placements of pipes once
    // they go off the left part of the screen
    if (xMaxRight < _topPipes[i].getRight()) {
      xMaxRight = _topPipes[i].getRight();
    }

    // Check for collisions and end of game
    if (_topPipes[i].overlaps(_bird) && !_isInvincible) {
      _topPipes[i].setDrawFill(true);
      _gameState = GAME_OVER;
      _gameOverTimestamp = millis();
    } else {
      _topPipes[i].setDrawFill(false);
    }

    if (_bottomPipes[i].overlaps(_bird) && !_isInvincible) {
      _bottomPipes[i].setDrawFill(true);
      _gameState = GAME_OVER;
      _gameOverTimestamp = millis();
    } else {
      _bottomPipes[i].setDrawFill(false);
    }
  }

  // Check for pipes that have gone off the screen to the left
  // and reset them to off the screen on the right
  xMaxRight = max(xMaxRight, _display.width());
  for (int i = 0; i < NUM_PIPES; i++) {
    if (_topPipes[i].getRight() < 0) {
      int pipeX = xMaxRight + random(MIN_PIPE_X_SPACING_DISTANCE, MAX_PIPE_X_SPACING_DISTANCE);
      int pipeWidth = random(MIN_PIPE_WIDTH, MAX_PIPE_WIDTH);

      int yGapBetweenPipes = random(MIN_PIPE_Y_SPACE, MAX_PIPE_Y_SPACE);

      int topPipeY = 0;
      int topPipeHeight = random(0, SCREEN_HEIGHT - yGapBetweenPipes);

      int bottomPipeY = topPipeHeight + yGapBetweenPipes;
      int bottomPipeHeight = SCREEN_HEIGHT - bottomPipeY;

      _topPipes[i].setLocation(pipeX, topPipeY);
      _topPipes[i].setDimensions(pipeWidth, topPipeHeight);
      _topPipes[i].setHasPassedBird(false);

      _bottomPipes[i].setLocation(pipeX, bottomPipeY);
      _bottomPipes[i].setDimensions(pipeWidth, bottomPipeHeight);
      _bottomPipes[i].setHasPassedBird(false);

      xMaxRight = _topPipes[i].getRight();
    }
  }

  _bird.draw(_display);
}


void showLoadScreen() {
  // Clear the buffer
  _display.clearDisplay();

  // Show load screen
  _display.setTextSize(1);
  _display.setTextColor(WHITE, BLACK);

  int16_t x1, y1;
  uint16_t w, h;
  _display.setTextSize(1);

  int yText = 10;
  _display.getTextBounds(STR_LOADSCREEN_CREATOR, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_LOADSCREEN_CREATOR);

  _display.setTextSize(2);
  yText = yText + h + 1;
  _display.getTextBounds(STR_LOADSCREEN_APP_NAME_LINE1, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_LOADSCREEN_APP_NAME_LINE1);

  yText = yText + h + 1;
  _display.getTextBounds(STR_LOADSCREEN_APP_NAME_LINE2, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_LOADSCREEN_APP_NAME_LINE2);

  _display.display();
  delay(LOAD_SCREEN_SHOW_MS);
  _display.clearDisplay();
  _display.setTextSize(1);

}

/**
 * Call this every frame to calculate frame rate
 */
void calcFrameRate() {
  unsigned long elapsedTime = millis() - _fpsStartTimeStamp;
  _frameCount++;
  if (elapsedTime > 1000) {
    _fps = _frameCount / (elapsedTime / 1000.0);
    _fpsStartTimeStamp = millis();
    _frameCount = 0;
  }
}

/**
 * Draws the status bar at top of screen with points and fps
 */
void drawStatusBar() {
  // Draw accumulated points
  _display.setTextSize(1);
  _display.setCursor(0, 0); // draw points
  _display.print(_points);

  // Draw frame count
  if (_drawFrameCount) {
    int16_t x1, y1;
    uint16_t w, h;
    _display.getTextBounds("XX.XX fps", 0, 0, &x1, &y1, &w, &h);
    _display.setCursor(_display.width() - w, 0);
    _display.print(_fps);
    _display.print(" fps");
  }
}
