
/**
 * Reads two potentiometers to control the hue and brightness of an
 * 8-LED WS2812B NeoPixel stick, and displays both values as text
 * on an SSD1306 OLED display.
 *
 * Pot 1 (A0): Controls hue (0-360°, mapped across the full color wheel)
 * Pot 2 (A1): Controls brightness (0-255)
 *
 * The OLED shows:
 *  - "HUE" header with the current value in degrees
 *  - "BRIGHTNESS" header with the current value as a percentage
 *
 * Wiring:
 *  - Pot 1 wiper   → A0
 *  - Pot 2 wiper   → A1
 *  - NeoPixel DIN  → Pin 2 (any digital pin works)
 *  - NeoPixel VCC  → 5V
 *  - NeoPixel GND  → GND
 *  - OLED SDA      → A4
 *  - OLED SCL      → A5
 *
 * For our NeoPixel tutorials, see:
 * https://makeabilitylab.github.io/physcomp/advancedio/addressable-leds.html
 *
 * For our OLED tutorials, see:
 * https://makeabilitylab.github.io/physcomp/advancedio/oled.html
 *
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

// Includes for OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Include for NeoPixels
#include <Adafruit_NeoPixel.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// NeoPixel configuration
const int NEOPIXEL_PIN = 2;
const int NUM_LEDS = 8;
Adafruit_NeoPixel _strip(NUM_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Potentiometer pins
const int HUE_POT_PIN = A0;
const int BRIGHTNESS_POT_PIN = A1;
const int MAX_ANALOG_VAL = 1023;

void setup()
{
  Serial.begin(9600);

  // Initialize the OLED
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  _display.clearDisplay();
  _display.setTextColor(SSD1306_WHITE);

  // Initialize the NeoPixel strip
  _strip.begin();
  _strip.setBrightness(255); // We control brightness via HSV, not setBrightness
  _strip.show();             // Initialize all pixels to off
}

void loop()
{
  // Read potentiometers (with brief delay for ADC multiplexer settling)
  int huePotVal = analogRead(HUE_POT_PIN);
  delay(1);
  int briPotVal = analogRead(BRIGHTNESS_POT_PIN);

  // Map pot values to hue (0-65535 for NeoPixel HSV) and brightness (0-255)
  uint16_t hue = map(huePotVal, 0, MAX_ANALOG_VAL, 0, 65535);
  uint8_t brightness = map(briPotVal, 0, MAX_ANALOG_VAL, 0, 255);

  // Hue in degrees (0-360) and brightness as percentage for display
  int hueDegrees = map(huePotVal, 0, MAX_ANALOG_VAL, 0, 360);
  int briPercent = map(brightness, 0, 255, 0, 100);

  // Set all LEDs to the same hue and brightness
  uint32_t color = _strip.gamma32(_strip.ColorHSV(hue, 255, brightness));
  _strip.fill(color);
  _strip.show();

  // Draw the OLED display
  _display.clearDisplay();

  // --- Hue ---
  _display.setTextSize(1);
  _display.setCursor(0, 0);
  _display.print(F("HUE"));

  // Display hue value large and centered
  String strHue = String(hueDegrees);
  drawCenteredText(strHue, 5, 2);

  // Draw a degree symbol as a small circle after the hue text
  int16_t x1, y1;
  uint16_t tw, th;
  _display.setTextSize(2);
  _display.getTextBounds(strHue, 0, 0, &x1, &y1, &tw, &th);
  int textX = (SCREEN_WIDTH - tw) / 2;
  int degreeRadius = 2;
  int degreeX = textX + tw + degreeRadius + 1;   // just past the text
  int degreeY = 10 + degreeRadius + 1;           // near the top of the text
  _display.drawCircle(degreeX, degreeY, degreeRadius, SSD1306_WHITE);

  // --- Brightness ---
  _display.setTextSize(1);
  _display.setCursor(0, 30);
  _display.print(F("BRIGHTNESS"));

  // Display brightness value large and centered
  String strBri = String(briPercent) + "%";
  drawCenteredText(strBri, 44, 2);

  _display.display();

  delay(10);
}

/**
 * Draws a string horizontally centered on the display at the given y position.
 *
 * @param text      The string to draw
 * @param y         The y position for the top of the text
 * @param textSize  The Adafruit GFX text size multiplier
 */
void drawCenteredText(const String &text, int y, int textSize) {
  _display.setTextSize(textSize);
  int16_t x1, y1;
  uint16_t textWidth, textHeight;
  _display.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);
  _display.setCursor((SCREEN_WIDTH - textWidth) / 2, y);
  _display.print(text);
}
