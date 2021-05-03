/**
 * A simple pong game for the OLED display
 *  
 * This code requires the Shape.hpp and ParallaxJoystick.hpp from the MakeabilityLab_Arduino_Library
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
 * https://makeabilitylab.github.io/p5js/Games/Pong/
 *  
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */
#include <Wire.h>
#include <SPI.h>
#include <Shape.hpp>;
#include <ParallaxJoystick.hpp>;

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED _display width, in pixels
#define SCREEN_HEIGHT 64 // OLED _display height, in pixels

// Declaration for an SSD1306 _display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int DELAY_LOOP_MS = 5;
const int JOYSTICK_UPDOWN_PIN = A1;
const int JOYSTICK_LEFTRIGHT_PIN = A0;
const int SERVE_BUTTON_INPUT_PIN = 5;

const int MAX_ANALOG_VAL = 1023;
const enum JoystickYDirection JOYSTICK_Y_DIR = RIGHT;

// Analog joystick for left player
ParallaxJoystick _analogJoystick(JOYSTICK_UPDOWN_PIN, JOYSTICK_LEFTRIGHT_PIN, MAX_ANALOG_VAL, JOYSTICK_Y_DIR);

// Digital buttons (joystick) for right player
const int UP_BUTTON_INPUT_PIN = 6;
const int DOWN_BUTTON_INPUT_PIN = 7;

Ball _ball(20, 20, 3);

#define PADDLE_WIDTH 4
#define PADDLE_HEIGHT 15

const char STR_PLAYER1_SERVE[] = "Player 1 serve!";
const char STR_PLAYER2_SERVE[] = "Player 2 serve!";
const char STR_PLAY_TO[] = "Play to ";
const char STR_GAME_POINT[] = "GAME POINT!";
const char STR_GAME_OVER[] = "Game Over";
const char STR_PLAYER1_WON[] = "Player 1 wins!";
const char STR_PLAYER2_WON[] = "Player 2 wins!";
const char STR_LOADSCREEN_CREATOR[] = "Makeability Lab";
const char STR_LOADSCREEN_GAME_NAME[] = "PONG!";

Rectangle _leftPaddle(0, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT);
Rectangle _rightPaddle(SCREEN_WIDTH - PADDLE_WIDTH, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT);

int _leftPlayerScore = 0;
int _rightPlayerScore = 0;

const int GAME_OVER_SCORE = 3;

enum GameState {
  NEW_GAME,
  BALL_SERVE,
  PLAYING,
  GAME_OVER,
};

enum Players {
  LEFT_PLAYER = 0,
  RIGHT_PLAYER = 1
};

enum GameState _curGameState = NEW_GAME;
enum Players _ballServer = LEFT_PLAYER;
enum Players _gameWinner = LEFT_PLAYER;

//Shape _shape(20, 20, 20, 20);

// for tracking fps
float _fps = 0;
unsigned long _frameCount = 0;
unsigned long _fpsStartTimeStamp = 0;
const boolean _drawFps = false; // change to show/hide fps display

void setup() {
  Serial.begin(9600);

  // Setup digital joystick input
  pinMode(UP_BUTTON_INPUT_PIN, INPUT_PULLUP);
  pinMode(DOWN_BUTTON_INPUT_PIN, INPUT_PULLUP);

  initializeOledAndShowLoadScreen();

  _ball.setDrawBoundingBox(false);
  _ball.setDrawFill(true);
  _leftPaddle.setDrawFill(true);
  _rightPaddle.setDrawFill(true);
  //Serial.println((String)"_xBall:" + _xBall + " _xBall:" + _xBall + " _xSpeed:" + _xSpeed + " _ySpeed:" + _ySpeed);

  // Randomly select who starts with ball
  // if analog input pin 5 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(A5));
  if (random(0, 2) >= 1) {
    _ballServer = RIGHT_PLAYER;
  }
  _fpsStartTimeStamp = millis();
}

void loop() {

  _display.clearDisplay();

  int16_t x1, y1;
  uint16_t w, h;
  if (_curGameState == NEW_GAME || _curGameState == BALL_SERVE) {

    int yText = 0;
    if (_ballServer == LEFT_PLAYER) {
      _display.getTextBounds(STR_PLAYER1_SERVE, 0, 0, &x1, &y1, &w, &h);
      yText = _display.height() / 2 - h / 2;
      _display.setCursor(_display.width() / 2 - w / 2, yText);
      _display.print(STR_PLAYER1_SERVE);
    } else {
      _display.getTextBounds(STR_PLAYER2_SERVE, 0, 0, &x1, &y1, &w, &h);
      yText = _display.height() / 2 - h / 2;
      _display.setCursor(_display.width() / 2 - w / 2, yText);
      _display.print(STR_PLAYER2_SERVE);
    }

    if(GAME_OVER_SCORE - _leftPlayerScore == 1 || GAME_OVER_SCORE - _rightPlayerScore == 1){
      yText = yText + h + 2;
      _display.getTextBounds(STR_GAME_POINT, 0, 0, &x1, &y1, &w, &h);
      _display.setCursor(_display.width() / 2 - w / 2, yText);
      _display.print(STR_GAME_POINT);
    }else{
      yText = yText + h + 2;
      _display.getTextBounds("Play to 3 pts", 0, 0, &x1, &y1, &w, &h);
      _display.setCursor(_display.width() / 2 - w / 2, yText);
      _display.print(STR_PLAY_TO);
      _display.print(GAME_OVER_SCORE);
      _display.print(" pts");
    }
  }

  // Draw fps
  if (_drawFps && _curGameState == PLAYING) {
    drawFrameRate();
  }
  calcFrameRate();

  // draw scores
  drawScores();

  _ball.update();

  int ballServeButtonVal = digitalRead(SERVE_BUTTON_INPUT_PIN);

  // Read digital joystick (buttons) to control right paddle
  int upButtonVal = digitalRead(UP_BUTTON_INPUT_PIN);
  int downButtonVal = digitalRead(DOWN_BUTTON_INPUT_PIN);

  if (upButtonVal == LOW) {
    _rightPaddle.setY(_rightPaddle.getY() - 2);
    //Serial.println((String)"Right paddle: upButtonVal:" + upButtonVal + " downButtonVal:" + downButtonVal);
  } else if (downButtonVal == LOW) {
    _rightPaddle.setY(_rightPaddle.getY() + 2);
    //Serial.println((String)"Right paddle: upButtonVal:" + upButtonVal + " downButtonVal:" + downButtonVal);
  }
  _rightPaddle.forceInside(0, 0, _display.width(), _display.height());

  // Read analog joystick to control left paddle
  _analogJoystick.read();
  int upDownVal = _analogJoystick.getUpDownVal();
  int yMovementPixels = map(upDownVal, 0, _analogJoystick.getMaxAnalogValue() + 1, -4, 5);

  _leftPaddle.setY(_leftPaddle.getY() - yMovementPixels);
  _leftPaddle.forceInside(0, 0, _display.width(), _display.height());

  // Check game play states and do appropriate behavior
  if (_curGameState == PLAYING) {
    // If we're here, then we're actively playing a game

    // Draw the court
    drawCourt();

    // Check for ball bounce off top and bottom of screen
    if (_ball.checkYBounce(0, _display.height() - 1)) {
      _ball.reverseYSpeed();
      _ball.forceInside(0, 0, _display.width(), _display.height());
    }

    // Check for ball-paddle collision. We could be far smarter here and actually
    // calculate the trajectory of the ball, etc. See:
    // https://codeincomplete.com/articles/javascript-pong/part4/
    // This is important because the ball can move multiple pixels per frame
    // Could also put "spin" on the ball and have the paddle's movement
    // affect the trajectory.
    //
    // But, for now, we'll just leave it as this is a rapid prototype for
    // demonstrative purposes
    if ( _leftPaddle.overlaps(_ball)) {
      //_display.setCursor(0, 20);
      //_display.print((String)"LP.R="+_leftPaddle.getRight() + "<= Ball.L=" + _ball.getLeft());

      _ball.setX(_leftPaddle.getRight());
      _ball.reverseXSpeed();

    }
    else if (_rightPaddle.overlaps(_ball)) {
      //_display.setCursor(0, 20);
      //_display.print((String)"RP.L="+_rightPaddle.getLeft() + ">= Ball.R=" + _ball.getRight());

      _ball.setX(_rightPaddle.getLeft() - _ball.getWidth());
      _ball.reverseXSpeed();
    }

    // Check ball off screen (and, hence, player scores!)
    if (_ball.getRight() <= 0) {
      _rightPlayerScore++;

      if (_rightPlayerScore >= GAME_OVER_SCORE) {
        _curGameState = GAME_OVER;
        _gameWinner = RIGHT_PLAYER;
      } else {
        _curGameState = BALL_SERVE;
      }
      _ballServer = RIGHT_PLAYER;
    } else if (_ball.getLeft() >= _display.width()) {
      _leftPlayerScore++;
      if (_leftPlayerScore >= GAME_OVER_SCORE) {
        _curGameState = GAME_OVER;
        _gameWinner = LEFT_PLAYER;
      } else {
        _curGameState = BALL_SERVE;
      }
      _ballServer = LEFT_PLAYER;
    }
  } else if (_curGameState == GAME_OVER) {
    drawGameOver();

    // Check to see if ball serve button is pressed
    // If so, go into a "new game" state
    if (ballServeButtonVal == LOW) {
      _leftPlayerScore = 0;
      _rightPlayerScore = 0;
      _curGameState = NEW_GAME;
    }
  } else {
    // If we're here, then we are waiting for a player to serve
    int xSpeed = random(1, _ball.getRadius() + 1);
    int ySpeed = random(1, _ball.getRadius() + 1);

    if (_ballServer == LEFT_PLAYER) {
      // Move the ball with the left player paddle
      _ball.setLocation(_leftPaddle.getRight(), _leftPaddle.getY() +
                        _leftPaddle.getHeight() / 2 - _ball.getHeight() / 2);

      // Now serve ball
      if (ballServeButtonVal == LOW) {
        _ball.setSpeed(xSpeed, ySpeed);
        _curGameState = PLAYING;
      }
    } else {
      // Move the ball with the right player paddle
      _ball.setLocation(_rightPaddle.getLeft() - _ball.getWidth(),
                        _rightPaddle.getY() + _rightPaddle.getHeight() / 2 - _ball.getHeight() / 2);

      // If ball serve button pressed, serve ball
      if (ballServeButtonVal == LOW) {
        _ball.setSpeed(-xSpeed, ySpeed);
        _curGameState = PLAYING;
      }
    }
  }

  _ball.draw(_display);
  _leftPaddle.draw(_display);
  _rightPaddle.draw(_display);

  // Render buffer to screen
  _display.display();

  if (DELAY_LOOP_MS > 0) {
    delay(DELAY_LOOP_MS);
  }


  //Serial.println((String)"x:" + _ball.getX() + " y:" + _ball.getY() + " xSpeed:" + _ball.getXSpeed() + " ySpeed:" + _ball.getYSpeed());
}

/**
 * Draw the scores to the display
 */
void drawScores(){
  //_display.getTextBounds((String)("" + _leftPlayerScore), 0, 0, &x1, &y1, &w, &h);
  //_display.setCursor(_display.width() / 4 - w / 2, 0);
  _display.setCursor(_display.width() / 4, 0);
  _display.print(_leftPlayerScore);

  //_display.getTextBounds((String)("" + _rightPlayerScore), 0, 0, &x1, &y1, &w, &h);
  _display.setCursor((int)(_display.width() * 0.75), 0);
  _display.print(_rightPlayerScore);
}

/**
 * Draw the "pong" (or tennis) court
 */
void drawCourt(){
  const int yDotSpacing = 5;
  const int xDotLine = _display.width() / 2;
  for (int yDot = yDotSpacing; yDot < _display.height(); yDot += yDotSpacing) {
    _display.drawPixel(xDotLine, yDot, SSD1306_WHITE);
  }
}

/**
 * Call this from setup() to initialize OLED display and show load screen
 */
void initializeOledAndShowLoadScreen() {
  // SSD1306_SWITCHCAPVCC = generate _display voltage from 3.3V internally
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  _display.clearDisplay();

  // Show load screen
  _display.setTextSize(1);
  _display.setTextColor(WHITE, BLACK);

  int16_t x1, y1;
  uint16_t w, h;
  _display.getTextBounds(STR_LOADSCREEN_CREATOR, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, 15);
  _display.print(STR_LOADSCREEN_CREATOR);

  _display.setTextSize(3);
  _display.getTextBounds(STR_LOADSCREEN_GAME_NAME, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, 25);
  _display.print(STR_LOADSCREEN_GAME_NAME);


  _display.display();
  delay(500);
  _display.clearDisplay();
  _display.setTextSize(1);
}

/**
   Draws game over screen to display
*/
void drawGameOver() {
  int16_t x1, y1;
  uint16_t w, h;
  
  _display.setTextSize(2);
  _display.getTextBounds(STR_GAME_OVER, 0, 0, &x1, &y1, &w, &h);

  int yGameOverText = 10;
  int yPlayerWonText = yGameOverText + h + 2;
  _display.setCursor(_display.width() / 2 - w / 2, yGameOverText);
  _display.print(STR_GAME_OVER);

  _display.setTextSize(1);
  if (_gameWinner == LEFT_PLAYER) {
    _display.getTextBounds(STR_PLAYER1_WON, 0, 0, &x1, &y1, &w, &h);
    _display.setCursor(_display.width() / 2 - w / 2, yPlayerWonText);
    _display.print(STR_PLAYER1_WON);
  } else {
    _display.getTextBounds(STR_PLAYER2_WON, 0, 0, &x1, &y1, &w, &h);
    _display.setCursor(_display.width() / 2 - w / 2, yPlayerWonText);
    _display.print(STR_PLAYER2_WON);
  }
}

/**
   Call this every frame to calculate frame rate
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
   Draw frame rate
*/
void drawFrameRate() {
  // Draw frame count
  int16_t x1, y1;
  uint16_t w, h;
  _display.getTextBounds("XX.XX fps", 0, 0, &x1, &y1, &w, &h);

  // Draw fps in bottom right corner
  _display.setCursor(_display.width() - w, _display.height()-h);
  _display.print(_fps);
  _display.print(" fps");
}
