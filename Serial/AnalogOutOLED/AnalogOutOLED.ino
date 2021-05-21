/**
 * Reads in an analog value and sends a normalized value [0, 1] inclusive
 * over the serial port with 4 decimal point precision.
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

const int ANALOG_INPUT_PIN = A0;
const int MAX_ANALOG_INPUT = 1023;

int _lastAnalogVal = -1;
float _curShapeSizeFraction = -1;

// If false, only sends new data when the new analog value does not
// equal the last analog value. If true, always sends the data
boolean _alwaysSendData = true; 

const int MIN_SHAPE_SIZE = 4;
int MAX_SHAPE_SIZE;

void setup() {
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  MAX_SHAPE_SIZE = min(_display.width(), _display.height());

  _display.clearDisplay();
  _display.setTextSize(1);      // Normal 1:1 pixel scale
  _display.setTextColor(SSD1306_WHITE); // Draw white text
  _display.setCursor(0, 0);     // Start at top-left corner
}

void loop() {
  _display.clearDisplay();

  int analogVal = analogRead(ANALOG_INPUT_PIN);
  int shapeSize = map(analogVal, 0, MAX_ANALOG_INPUT, MIN_SHAPE_SIZE, MAX_SHAPE_SIZE);
  int radius = shapeSize / 2;
  int xCenter = _display.width() / 2;
  int yCenter = _display.height() / 2; 

  _display.fillCircle(xCenter, yCenter, radius, SSD1306_WHITE);
  _display.display();

  // If the analog value has changed, send a new one over serial
  if(_alwaysSendData || _lastAnalogVal != analogVal){
    float sizeFrac = analogVal / (float)MAX_ANALOG_INPUT;
    Serial.println(sizeFrac, 4); // 4 decimal point precision
  }

  _lastAnalogVal = analogVal;
  delay(DELAY_MS);
}
