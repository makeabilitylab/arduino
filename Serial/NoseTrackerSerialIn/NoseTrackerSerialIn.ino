/**
 * Moves a face around the screen based on an x,y normalized position
 * read off of the serial port. The x,y positions are normalized to
 * [0, 1] inclusive as floating points. So, for example, 0.5, 0.5 would
 * be the center of the screen
 * 
 * This sketch is designed to work with the p5.js app NoseTracker:
 * - Live app: https://makeabilitylab.github.io/p5js/WebSerial/p5js/NoseTracker/
 * - Code: https://github.com/makeabilitylab/p5js/tree/master/WebSerial/p5js/NoseTracker
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

const int DELAY_MS = 5;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int CHAR_SIZE = 3;           // set font size to 3
const int DEFAULT_CHAR_WIDTH = 5;  // default font is 5 pixels wide at size 1
const int DEFAULT_CHAR_HEIGHT = 8; // default font is 8 pixels wide at size 1

int _charWidth = DEFAULT_CHAR_WIDTH * CHAR_SIZE;   // calculate char width at char size
int _charHeight = DEFAULT_CHAR_HEIGHT * CHAR_SIZE; // calculate char width at char size

float _faceX = 0; // normalized x position of face
float _faceY = 0; // normalize y position of face

void setup() {
  Serial.begin(115200);

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
      String strXLocation = rcvdSerialData.substring(0, indexOfComma);
      _faceX = strXLocation.toFloat();

      String strYLocation = rcvdSerialData.substring(indexOfComma + 1, rcvdSerialData.length());
      _faceY = strYLocation.toFloat();
    } 
    
    // Echo the data back on serial (for debugging purposes)
    // This is not necessary but helpful. Then the webpage can
    // display this debug output (if necessary)
    // Prefix debug output with '#' as a convention
    Serial.print("# Arduino Received: '");
    Serial.print(rcvdSerialData);
    Serial.println("'");
  }

  _display.clearDisplay();
  drawFace(_faceX, _faceY);
  _display.display();
  delay(DELAY_MS);
}

/**
 * Draws a face at the given x,y fractions [0, 1] mapped to screen
 */
void drawFace(float xFrac, float yFrac){
  int x = xFrac * (_display.width() - _charWidth);
  int y = yFrac * (_display.height() - _charHeight);
  
  _display.drawChar(x, y, (unsigned char)2, SSD1306_WHITE, SSD1306_BLACK, CHAR_SIZE);
}
