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

const long BAUD_RATE = 115200;
void setup() {
  Serial.begin(BAUD_RATE);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

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
      // Parse out the shape type, which should be 0 (circle), 1 (square), 2 (triangle)
      String strShapeType = rcvdSerialData.substring(0, indexOfComma);
      int shapeType = strShapeType.toInt();
    
      // Parse out shape size fraction, a float between [0, 1]
      String strShapeSize = rcvdSerialData.substring(indexOfComma + 1, rcvdSerialData.length());
      float curShapeSizeFraction = strShapeSize.toFloat();
    
      // Display data for debugging purposes
      _display.clearDisplay();
      _display.setCursor(0, 0);
      _display.println("RECEIVED:");
      _display.println(rcvdSerialData);
    
      // Display parsed data
      _display.println("\nPARSED:");
      _display.print("Shape Type: ");
      _display.println(shapeType);
      _display.print("Shape Size: ");
      _display.print(curShapeSizeFraction);
      _display.display();
    } 

    // Echo the data back on serial (for debugging purposes)
    Serial.print("Arduino Received: '");
    Serial.print(rcvdSerialData);
    Serial.println("'");
  }
}
