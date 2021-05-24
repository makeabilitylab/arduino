/**
 * Takes in three parameters off of serial as comma separated text-encoded
 * data: shapeType, shapeSize, drawMode and draws a shape accordingly.
 * 
 * shapeType is either 0, 1, 2 corresponding to CIRCLE, SQUARE, TRIANGLE
 * shapeSize is a float between [0, 1] inclusive that corresponds to shape size
 * drawMode is either 0, 1 corresponding to FILL, OUTLINE
 * 
 * Designed to work with the p5.js app:
 *  - Live page: http://makeabilitylab.github.io/p5js/WebSerial/p5js/DisplayShapeBidirectional
 *  - Code: https://github.com/makeabilitylab/p5js/tree/master/WebSerial/p5js/DisplayShapeBidirectional
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

// #include <SPI.h> // Comment out when using i2c
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

enum ShapeType {
  CIRCLE,
  SQUARE,
  TRIANGLE,
  NUM_SHAPE_TYPES
};

ShapeType _curShapeType = CIRCLE;

enum DrawMode{
  FILL,
  OUTLINE,
  NUM_DRAW_MODES
};

DrawMode _curDrawMode = FILL;

float _curShapeSizeFraction = 0.5;

const int MIN_SHAPE_SIZE = 4;
int _maxShapeSize;

const int SHAPE_SELECTION_BUTTON_PIN = 4;
const int SHAPE_DRAWMODE_BUTTON_PIN = 5;

int _lastShapeSelectionButtonVal = HIGH;
int _lastDrawModeButtonVal = HIGH;

const long BAUD_RATE = 115200;
void setup() {
  Serial.begin(BAUD_RATE);

  pinMode(SHAPE_SELECTION_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SHAPE_DRAWMODE_BUTTON_PIN, INPUT_PULLUP);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  _maxShapeSize = min(_display.width(), _display.height());

  _display.clearDisplay();
  _display.setTextSize(1);      // Normal 1:1 pixel scale
  _display.setTextColor(SSD1306_WHITE); // Draw white text
  _display.setCursor(0, 0);     // Start at top-left corner
  _display.print("Waiting to receive\ndata from serial...");
  _display.println("\n");
  _display.print("Baud rate:");
  _display.print(BAUD_RATE);
  _display.print(" bps");
  _display.display();
}

void loop() {
  checkButtonPresses();

  // If we've received data from serial, then _curShapeSizeFraction will
  // no longer be -1
  if(_curShapeSizeFraction >= 0){
    drawShape(_curShapeType, _curShapeSizeFraction, _curDrawMode);
  }
}

/**
 * Check the shape selection and draw mode buttons and update the global 
 * variables _curShapeType and _curDrawMode accordingly
 */
void checkButtonPresses(){
  // Read the shape selection button (active LOW)
  int shapeSelectionButtonVal = digitalRead(SHAPE_SELECTION_BUTTON_PIN);
  int lastShapeType = _curShapeType;
  if(shapeSelectionButtonVal == LOW && shapeSelectionButtonVal != _lastShapeSelectionButtonVal){
    // Increment the shape type
    _curShapeType = (ShapeType)((int)_curShapeType + 1);

    // Reset back to CIRCLE if we've made it to NUM_SHAPE_TYPES
    if(_curShapeType >= NUM_SHAPE_TYPES){
      _curShapeType = CIRCLE;
    }
  }

  // Read the shape draw mode button val (active LOW)
  int shapeDrawModeButtonVal = digitalRead(SHAPE_DRAWMODE_BUTTON_PIN);
  int lastDrawMode = _curDrawMode;
  if(shapeDrawModeButtonVal == LOW && shapeDrawModeButtonVal != _lastDrawModeButtonVal){
    // Increment the draw mode
    _curDrawMode = (DrawMode)((int)_curDrawMode + 1);

    // Reset back to FILL if we've made it to NUM_DRAW_MODES
    if(_curDrawMode >= NUM_DRAW_MODES){
      _curDrawMode = FILL;
    }
  }

  // Send new shape type and shape draw mode back over serial
  if(lastShapeType != _curShapeType || lastDrawMode != _curDrawMode){
    Serial.print(_curShapeType);
    Serial.print(", ");
    Serial.println(_curDrawMode);
  }

  // Set last button values (so nothing happens if user just holds down a button)
  _lastShapeSelectionButtonVal = shapeSelectionButtonVal;
  _lastDrawModeButtonVal = shapeDrawModeButtonVal;
}

void drawShape(ShapeType shapeType, float fractionSize, DrawMode curDrawMode){
  _display.clearDisplay();

  int shapeSize = MIN_SHAPE_SIZE + fractionSize * (_maxShapeSize - MIN_SHAPE_SIZE);
  int halfShapeSize = shapeSize / 2;
  int xCenter = _display.width() / 2;
  int yCenter = _display.height() / 2; 
  int xLeft =  xCenter - halfShapeSize;
  int yTop =  yCenter - halfShapeSize;
  
  if(shapeType == CIRCLE){
    if(curDrawMode == FILL){
      _display.fillRoundRect(xLeft, yTop, shapeSize, shapeSize, halfShapeSize, SSD1306_WHITE);
    }else{
      _display.drawRoundRect(xLeft, yTop, shapeSize, shapeSize, halfShapeSize, SSD1306_WHITE);
    }
  }else if(shapeType == SQUARE){
    if(curDrawMode == FILL){
      _display.fillRect(xLeft, yTop, shapeSize, shapeSize, SSD1306_WHITE);
    }else{
      _display.drawRect(xLeft, yTop, shapeSize, shapeSize, SSD1306_WHITE);
    }
  }else if(shapeType == TRIANGLE){
    int x1 = xCenter - halfShapeSize;
    int y1 = yCenter + halfShapeSize;

    int x2 = xCenter;
    int y2 = yCenter - halfShapeSize;

    int x3 = xCenter + halfShapeSize;
    int y3 = y1;

    if(curDrawMode == FILL){
      _display.fillTriangle(x1, y1, x2, y2, x3, y3, SSD1306_WHITE);
    }else{
      _display.drawTriangle(x1, y1, x2, y2, x3, y3, SSD1306_WHITE);
    }
  }

  _display.display();
}
