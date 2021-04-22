#include <Wire.h>
#include <SPI.h>
#include <Shape.hpp>;

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED _display width, in pixels
#define SCREEN_HEIGHT 64 // OLED _display height, in pixels

// Declaration for an SSD1306 _display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char STR_LOADSCREEN_CREATOR[] = "Makeability Lab";
const char STR_LOADSCREEN_APP_NAME[] = "Flappy Bird!";
const int FLAP_BUTTON_INPUT_PIN = 5;


// for tracking fps
unsigned long _totalFrameCount = 0;
unsigned long _startTimeStamp = 0;

// status bar
const boolean _drawFrameCount = true; // change to show/hide status bar
const int DELAY_LOOP_MS = 5;
const int LOAD_SCREEN_SHOW_MS = 500;

class Bird : public Rectangle {
  public:
    Bird(int x, int y, int width, int height) : Rectangle(x, y, width, height)
    {
    }
};

const int BIRD_HEIGHT = 5;
const int BIRD_WIDTH = 10;
const int NUM_PIPES = 3;

const int MIN_PIPE_WIDTH = 8;
const int MAX_PIPE_WIDTH = 13; // in pixels
const int MIN_PIPE_X_SPACING_DISTANCE = BIRD_WIDTH * 2; // in pixels
const int MAX_PIPE_X_SPACING_DISTANCE = 100; // in pixels
const int MIN_PIPE_Y_SPACE = BIRD_HEIGHT * 2 + 3;
const int MAX_PIPE_Y_SPACE = SCREEN_HEIGHT - BIRD_HEIGHT * 2;

int _pipeSpeed = 2;
int _gravity = 1; // can't apply gravity every frame, apply every X time

Rectangle _topPipes[NUM_PIPES] = { Rectangle(0, 0, 0, 0),
                                   Rectangle(0, 0, 0, 0),
                                   Rectangle(0, 0, 0, 0)
                                 };

Rectangle _bottomPipes[NUM_PIPES] = { Rectangle(0, 0, 0, 0),
                                   Rectangle(0, 0, 0, 0),
                                   Rectangle(0, 0, 0, 0)
                                 };

Bird _bird(5, SCREEN_HEIGHT / 2, BIRD_WIDTH, BIRD_HEIGHT);

void setup() {
  Serial.begin(9600);

  pinMode(FLAP_BUTTON_INPUT_PIN, INPUT_PULLUP);

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

  _bird.setDrawFill(true);

  // Setup pipes
  const int minStartXPipeLocation = _display.width() / 2;
  int lastPipeX = minStartXPipeLocation;
  for(int i = 0; i < NUM_PIPES; i++){
    
    int pipeX = lastPipeX + random(MIN_PIPE_X_SPACING_DISTANCE, MAX_PIPE_X_SPACING_DISTANCE);
    int pipeWidth = random(MIN_PIPE_WIDTH, MAX_PIPE_WIDTH);
    
    int yGapBetweenPipes = random(MIN_PIPE_Y_SPACE, MAX_PIPE_Y_SPACE);

    int topPipeY = 0;
    int topPipeHeight = random(0, SCREEN_HEIGHT - yGapBetweenPipes);

    int bottomPipeY = topPipeHeight + yGapBetweenPipes;
    int bottomPipeHeight = SCREEN_HEIGHT - bottomPipeY;

    _topPipes[i].setLocation(pipeX, topPipeY);
    _topPipes[i].setDimensions(pipeWidth, topPipeHeight);

    _bottomPipes[i].setLocation(pipeX, bottomPipeY);
    _bottomPipes[i].setDimensions(pipeWidth, bottomPipeHeight);
    
    lastPipeX = _topPipes[i].getRight();
  }
}

void loop() {
  if (_startTimeStamp == 0) {
    _startTimeStamp = millis();
  }

  _display.clearDisplay();

  drawFrameCount();

  int flapButtonVal = digitalRead(FLAP_BUTTON_INPUT_PIN);
  _bird.setY(_bird.getY() + _gravity);
  
  if (flapButtonVal == LOW) {
    _bird.setY(_bird.getY() - 3);
  }
  _bird.forceInside(0, 0, _display.width(), _display.height());

  int xMaxRight = 0;
  for(int i = 0; i < NUM_PIPES; i++){

    _topPipes[i].setX(_topPipes[i].getX() - _pipeSpeed);
    _bottomPipes[i].setX(_bottomPipes[i].getX() - _pipeSpeed);

    _topPipes[i].draw(_display);
    _bottomPipes[i].draw(_display);

    Serial.println(_topPipes[i].toString());

    if(xMaxRight < _topPipes[i].getRight()){
      xMaxRight = _topPipes[i].getRight();
    }

    if(_topPipes[i].overlaps(_bird)){
      _topPipes[i].setDrawFill(true);
    }else{
      _topPipes[i].setDrawFill(false);
    }

    if(_bottomPipes[i].overlaps(_bird)){
      _bottomPipes[i].setDrawFill(true);
    }else{
      _bottomPipes[i].setDrawFill(false);
    }
  }

  // Check for pipes that have gone off the screen to the left
  // and reset them to off the screen on the right
  xMaxRight = max(xMaxRight, _display.width());
  for(int i = 0; i < NUM_PIPES; i++){
    if(_topPipes[i].getRight() < 0){
      int pipeX = xMaxRight + random(MIN_PIPE_X_SPACING_DISTANCE, MAX_PIPE_X_SPACING_DISTANCE);
      int pipeWidth = random(MIN_PIPE_WIDTH, MAX_PIPE_WIDTH);
      
      int yGapBetweenPipes = random(MIN_PIPE_Y_SPACE, MAX_PIPE_Y_SPACE);
  
      int topPipeY = 0;
      int topPipeHeight = random(0, SCREEN_HEIGHT - yGapBetweenPipes);
  
      int bottomPipeY = topPipeHeight + yGapBetweenPipes;
      int bottomPipeHeight = SCREEN_HEIGHT - bottomPipeY;

      _topPipes[i].setLocation(pipeX, topPipeY);
      _topPipes[i].setDimensions(pipeWidth, topPipeHeight);
  
      _bottomPipes[i].setLocation(pipeX, bottomPipeY);
      _bottomPipes[i].setDimensions(pipeWidth, bottomPipeHeight);

      xMaxRight = _topPipes[i].getRight();
    }
  }
  
  
  _bird.draw(_display);

  _display.display();
  _totalFrameCount++;

  if (DELAY_LOOP_MS > 0) {
    delay(DELAY_LOOP_MS);
  }
}


void showLoadScreen() {
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

  _display.setTextSize(2);
  _display.getTextBounds(STR_LOADSCREEN_APP_NAME, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, 25);
  _display.print(STR_LOADSCREEN_APP_NAME);

  _display.display();
  delay(LOAD_SCREEN_SHOW_MS);
  _display.clearDisplay();
  _display.setTextSize(1);

}

void drawFrameCount() {
  // Draw frame count
  if (_drawFrameCount && _totalFrameCount > 0) {
    int16_t x1, y1;
    uint16_t w, h;
    unsigned long elapsedTime = millis() - _startTimeStamp;
    float fps = _totalFrameCount / (elapsedTime / 1000.0);
    _display.getTextBounds("XX.XX fps", 0, 0, &x1, &y1, &w, &h);
    _display.setCursor(_display.width() - w, 0);
    _display.print(fps);
    _display.print(" fps");
  }
}
