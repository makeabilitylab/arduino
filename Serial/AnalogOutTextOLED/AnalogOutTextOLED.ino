/**
 * Reads in an analog value and sends over the serial port either:
 * 1. A normalized value [0, 1] inclusive with 4 decimal point precision
 * 2. The raw analog value (e.g., 0-1023)
 *
 * Also displays either the normalized val on the OLED or the raw analog val
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

const int DELAY_MS = 50;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int ANALOG_INPUT_PIN = A0;
const int MAX_ANALOG_INPUT = 1023;

int _lastAnalogVal = -1;

// If false, only sends new data when the new analog value does not
// equal the last analog value. If true, always sends the data
boolean _alwaysSendData = true; 

// Send fractional value or raw analog value
boolean _transmitFraction = false;

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
}

void loop() {
  
  int analogVal = analogRead(ANALOG_INPUT_PIN);

  // If the analog value has changed, send a new one over serial
  if(_alwaysSendData || _lastAnalogVal != analogVal){
    float valFrac = analogVal / (float)MAX_ANALOG_INPUT;

    int16_t x1, y1;
    uint16_t textWidth, textHeight;

    _display.setTextSize(3);
    String strAnalogVal = (String)analogVal;
    if(_transmitFraction){
      _display.getTextBounds("0.0000", 0, 0, &x1, &y1, &textWidth, &textHeight);
    }else{
      _display.getTextBounds(strAnalogVal, 0, 0, &x1, &y1, &textWidth, &textHeight);
    }
    
    uint16_t yText = _display.height() / 2 - textHeight / 2;
    uint16_t xText = _display.width() / 2 - textWidth / 2;

    _display.clearDisplay();
    _display.setCursor(xText, yText);
    if(_transmitFraction){
      _display.println(valFrac, 4);
      Serial.println(valFrac, 4); // 4 decimal point precision
    }else{
      _display.println(strAnalogVal);
      Serial.println(analogVal);
    }
    _display.display();
  }

  _lastAnalogVal = analogVal;
  delay(DELAY_MS);
}
