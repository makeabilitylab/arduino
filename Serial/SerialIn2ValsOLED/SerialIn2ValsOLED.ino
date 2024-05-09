
/**
 *  Reads in two inputs off of serial and (1) sets two LED brightnesses accordingly;
 * (2) draws a circle in the x,y position on OLED
 *
 *  By Jon E. Froehlich
 *  @jonfroehlich
 *  http://makeabilitylab.io
 *
 */

#include <Wire.h>
#include <SPI.h>

// For graphics libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int DELAY_LOOP_MS = 10; 
const int LED1_OUTPUT_PIN = 5; // PWM pin
const int LED2_OUTPUT_PIN = 6;

// On the Arduino Uno/Leonardo, there is a 10-bit ADC so the
// maximum analog value is 1023. On other microcontrollers, like the ESP32,
// there is a 12-bit ADC, so the maximum analog value is 4095
const int MAX_ANALOG_VAL = 1023; 
const int MAX_PWM_VAL = 255;

int _ballRadius = 4;

const long BAUD_RATE = 115200;
void setup() {
  pinMode(LED1_OUTPUT_PIN, OUTPUT);
  pinMode(LED2_OUTPUT_PIN, OUTPUT);

  Serial.begin(BAUD_RATE);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Clear the buffer
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
    // We expect a format like "x, y\n"
    int indexOfComma = rcvdSerialData.indexOf(',');
    
    if(indexOfComma != -1){
      String strVal1 = rcvdSerialData.substring(0, indexOfComma);
      float floatVal1 = strVal1.toFloat();
      
      String strVal2 = rcvdSerialData.substring(indexOfComma + 1, rcvdSerialData.length());
      float floatVal2 = strVal2.toFloat();

      // Set LEDs
      setLeds(floatVal1, floatVal2);

      // Clear display, prepare for next render
      _display.clearDisplay();
      drawBallPosition(floatVal1, floatVal2);
      drawStatusBar(floatVal1, floatVal2);

      // Render buffer to screen
      _display.display();

      // For debugging
      Serial.print("Received: '");
      Serial.print(rcvdSerialData);
      Serial.print("'");
      Serial.print(" Parsed to: ");
      Serial.print(floatVal1, 4);
      Serial.print(", ");
      Serial.println(floatVal2, 4);
    }else{
      Serial.print("Data in unexpected format! ");
      Serial.print(" Received: '");
      Serial.println(rcvdSerialData);
    } 
  }
  delay(DELAY_LOOP_MS);
}

void setLeds(float led1, float led2){
  int led1PwmVal = led1 * MAX_PWM_VAL;
  int led2PwmVal = led2 * MAX_PWM_VAL;
  analogWrite(LED1_OUTPUT_PIN, led1PwmVal);
  analogWrite(LED2_OUTPUT_PIN, led2PwmVal);
}

void drawBallPosition(float xPos, float yPos){
    // Set new circle location
    int xBall = _ballRadius + xPos * (_display.width() - 2 * _ballRadius);
    int yBall = _ballRadius + yPos * (_display.height() - 2 * _ballRadius);

    _display.fillCircle(xBall, yBall, _ballRadius, SSD1306_WHITE);
}

void drawStatusBar(float xPos, float yPos){
  // Display this normalized location to screen
  _display.setCursor(0, 0);
  _display.println("Normalized x,y:");
  _display.print("X: ");
  _display.println(xPos, 4);
  _display.print("Y: ");
  _display.print(yPos, 4);
}
