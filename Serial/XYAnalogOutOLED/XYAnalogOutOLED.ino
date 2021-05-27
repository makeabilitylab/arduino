
/**
 *  Read in two analog inputs, translate them to X,Y float positions
 *  between [0, 1] inclusive, and send over Serial. Also show the x,y
 *  location on the OLED screen.
 *
 *  Adafruit Gfx Library:
 *  https://learn.adafruit.com/adafruit-gfx-graphics-library/overview 
 *
 *  Adafruit OLED tutorials:
 *  https://learn.adafruit.com/monochrome-oled-breakouts
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
const int X_ANALOG_INPUT_PIN = A0;
const int Y_ANALOG_INPUT_PIN = A1;

// On the Arduino Uno/Leonardo, there is a 10-bit ADC so the
// maximum analog value is 1023. On other microcontrollers, like the ESP32,
// there is a 12-bit ADC, so the maximum analog value is 4095
const int MAX_ANALOG_VAL = 1023; 

float _x;
float _y;
int _radius = 4;

const long BAUD_RATE = 115200;
void setup() {
  
  Serial.begin(BAUD_RATE);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  _display.clearDisplay();
  _display.setTextSize(1);
  _display.setTextColor(WHITE, BLACK);
  _display.setCursor(0, 0);
  _display.println("Screen initialized!");
  _display.println("\n");
  _display.print("Baud rate:");
  _display.print(BAUD_RATE);
  _display.print(" bps");
  _display.display();
  delay(250);

  _x = _display.width() / 2;
  _y = _display.height() / 2;
}

void loop() {

  // Clear display, prepare for next render
  _display.clearDisplay();

  // Read the analog values
  int xAnalogVal = analogRead(X_ANALOG_INPUT_PIN);
  int yAnalogVal = analogRead(Y_ANALOG_INPUT_PIN);

  // Calculate normalized x,y location
  _x = xAnalogVal / (float)MAX_ANALOG_VAL;
  _y = yAnalogVal / (float)MAX_ANALOG_VAL;

  // Set new circle location based on accel
  int xBall = _radius + _x * (_display.width() - 2 * _radius);
  int yBall = _radius + _y * (_display.height() - 2 * _radius);

  // Display this normalized location to screen
  _display.setCursor(0, 0);
  _display.println("Normalized x,y:");
  _display.print("X: ");
  _display.println(_x, 4);
  _display.print("Y: ");
  _display.print(_y, 4);

  _display.fillCircle(xBall, yBall, _radius, SSD1306_WHITE);

  // Render buffer to screen
  _display.display();

  // Transmit over serial
  Serial.print(_x, 4);
  Serial.print(", ");
  Serial.println(_y, 4);
  
  if(DELAY_LOOP_MS > 0){
    delay(DELAY_LOOP_MS);
  }
}
