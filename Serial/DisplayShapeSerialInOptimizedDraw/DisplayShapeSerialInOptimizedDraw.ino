/**
 * Takes in two parameters off of serial as comma separated text-encoded
 * data: shapeType, shapeSize
 * 
 * shapeType is either 0, 1, 2 corresponding to CIRCLE, SQUARE, TRIANGLE
 * shapeSize is a float between [0, 1] inclusive that corresponds to shape size
 * 
 * Designed to work with the p5.js app:
 *  - Live page: https://makeabilitylab.github.io/p5js/WebSerial/p5js/DisplayShapeOut/
 *  - Code: https://github.com/makeabilitylab/p5js/tree/master/WebSerial/p5js/DisplayShapeOut
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
};

ShapeType _curShapeType = CIRCLE;
float _curShapeSizeFraction = -1;

int _lastShapeType = CIRCLE;
int _lastShapeSize = -1;

const int MIN_SHAPE_SIZE = 4;
int _maxShapeSize;

const long BAUD_RATE = 115200;
void setup() {
  Serial.begin(BAUD_RATE);

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
  // Check to see if there is any incoming serial data
  if(Serial.available() > 0){
    // If we're here, then serial data has been received
    // Read data off the serial port until we get to the endline delimiter ('\n')
    // Store all of this data into a string
    String rcvdSerialData = Serial.readStringUntil('\n'); 

    // Parse out the comma separated string
    int indexOfComma = rcvdSerialData.indexOf(',');
    
    if(indexOfComma != -1){
      String strShapeType = rcvdSerialData.substring(0, indexOfComma);
      int shapeType = strShapeType.toInt();
      _curShapeType = (ShapeType)shapeType;

      String strShapeSize = rcvdSerialData.substring(indexOfComma + 1, rcvdSerialData.length());
      _curShapeSizeFraction = strShapeSize.toFloat();
    } 
    
    // Echo the data back on serial (for debugging purposes)
    // This is not necessary but helpful. Then the webpage can
    // display this debug output (if necessary)
    Serial.print("Arduino Received: '");
    Serial.print(rcvdSerialData);
    Serial.println("'");
  }

  // if no data has arrived, shape fraction will be < 0
  if(_curShapeSizeFraction > 0){ 
    drawShape(_curShapeType, _curShapeSizeFraction);
  }
}

void drawShape(ShapeType shapeType, float fractionSize){
  
  int shapeSize = MIN_SHAPE_SIZE + fractionSize * (_maxShapeSize - MIN_SHAPE_SIZE);
  int halfShapeSize = shapeSize / 2;
  int xCenter = _display.width() / 2;
  int yCenter = _display.height() / 2; 
  int xLeft =  xCenter - halfShapeSize;
  int yTop =  yCenter - halfShapeSize;

  if(_lastShapeSize != shapeSize || _lastShapeType != shapeType){
    _display.clearDisplay();
    
    // Render the appropriate shape
    if(shapeType == CIRCLE){
      _display.fillRoundRect(xLeft, yTop, shapeSize, shapeSize, halfShapeSize, SSD1306_WHITE);
    }else if(shapeType == SQUARE){
      _display.fillRect(xLeft, yTop, shapeSize, shapeSize, SSD1306_WHITE);
    }else if(shapeType == TRIANGLE){
      int x1 = xCenter - halfShapeSize;
      int y1 = yCenter + halfShapeSize;
  
      int x2 = xCenter;
      int y2 = yCenter - halfShapeSize;
  
      int x3 = xCenter + halfShapeSize;
      int y3 = y1;
  
      _display.fillTriangle(x1, y1, x2, y2, x3, y3, SSD1306_WHITE);
    }
  
    _display.display();
  }
  
  _lastShapeType = shapeType;
  _lastShapeSize = shapeSize;
}
