
/**
 * Reads two potentiometers to control the hue and brightness of an
 * 8-LED WS2812B NeoPixel stick, and displays both values on an
 * SSD1306 OLED display with bar graphs, numeric values, raw ADC
 * readings, and an approximate color name for the current hue.
 *
 * Pot 1 (A0): Controls hue (0-360°, mapped across the full color wheel)
 * Pot 2 (A1): Controls brightness (0-255)
 *
 * Wiring:
 *  - Pot 1 wiper   → A0
 *  - Pot 2 wiper   → A1
 *  - NeoPixel DIN  → Pin 7 (avoid pin 2 on Leonardo; it's I2C SDA)
 *  - NeoPixel VCC  → 5V
 *  - NeoPixel GND  → GND
 *  - OLED SDA      → A4
 *  - OLED SCL      → A5
 *
 * For our NeoPixel tutorial, see:
 * https://makeabilitylab.github.io/physcomp/advancedio/addressable-leds.html
 *
 * For our OLED tutorial, see:
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
const int NEOPIXEL_PIN = 7; // Can't use PIN 2 on Leonardo because used by SDA
const int NUM_LEDS = 8;
Adafruit_NeoPixel _strip(NUM_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Potentiometer pins
const int HUE_POT_PIN = A0;
const int BRIGHTNESS_POT_PIN = A1;
const int MAX_ANALOG_VAL = 1023;

// Bar graph layout constants
const int BAR_X = 0;
const int BAR_WIDTH = SCREEN_WIDTH;
const int BAR_HEIGHT = 7;

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
  _display.setTextSize(1);

  // ========== HUE SECTION (top half, y = 0..29) ==========

  // Row 1 (y=0): "HUE" label on the left, color name on the right
  _display.setCursor(0, 0);
  _display.print(F("HUE"));

  const char* colorName = getColorName(hueDegrees);
  drawRightAlignedText(colorName, 0);

  // Row 2 (y=10): Bar graph showing hue as fraction of 360
  int hueBarFill = map(hueDegrees, 0, 360, 0, BAR_WIDTH - 2);
  drawBarGraph(BAR_X, 10, BAR_WIDTH, BAR_HEIGHT, hueBarFill);

  // Row 3 (y=19): Degree value on the left, raw ADC on the right
  _display.setCursor(0, 19);
  _display.print(hueDegrees);

  // Draw degree symbol as a small circle right after the number
  int16_t x1, y1;
  uint16_t tw, th;
  _display.getTextBounds(String(hueDegrees), 0, 0, &x1, &y1, &tw, &th);
  _display.drawCircle(tw + 2, 19 + 1, 1, SSD1306_WHITE);

  drawRightAlignedText("raw:" + String(huePotVal), 19);

  // ========== BRIGHTNESS SECTION (bottom half, y = 32..63) ==========

  // Row 1 (y=32): "BRI" label on the left
  _display.setCursor(0, 32);
  _display.print(F("BRI"));

  drawRightAlignedText(String(briPercent) + "%", 32);

  // Row 2 (y=42): Bar graph showing brightness as fraction of 255
  int briBarFill = map(brightness, 0, 255, 0, BAR_WIDTH - 2);
  drawBarGraph(BAR_X, 42, BAR_WIDTH, BAR_HEIGHT, briBarFill);

  // Row 3 (y=51): Brightness value on the left, raw ADC on the right
  _display.setCursor(0, 51);
  _display.print(brightness);
  _display.print(F("/255"));

  drawRightAlignedText("raw:" + String(briPotVal), 51);

  _display.display();

  delay(10);
}

/**
 * Draws a horizontal bar graph: an outlined rectangle with a filled
 * portion representing the current value.
 *
 * @param x       Top-left x of the bar
 * @param y       Top-left y of the bar
 * @param w       Total width of the bar outline
 * @param h       Height of the bar
 * @param fillW   Width of the filled portion (0 to w-2)
 */
void drawBarGraph(int x, int y, int w, int h, int fillW) {
  _display.drawRect(x, y, w, h, SSD1306_WHITE);
  if(fillW > 0) {
    _display.fillRect(x + 1, y + 1, fillW, h - 2, SSD1306_WHITE);
  }
}

/**
 * Draws a string right-aligned against the right edge of the display
 * at the given y position. Assumes text size is already set.
 *
 * @param text  The string to draw
 * @param y     The y position for the top of the text
 */
void drawRightAlignedText(const String &text, int y) {
  int16_t x1, y1;
  uint16_t tw, th;
  _display.getTextBounds(text, 0, 0, &x1, &y1, &tw, &th);
  _display.setCursor(SCREEN_WIDTH - tw, y);
  _display.print(text);
}

/**
 * Returns an approximate color name for a given hue angle (0-360).
 * Uses 12 named color regions spanning the HSV color wheel.
 *
 * @param hueDeg  Hue in degrees (0-360)
 * @return        A short color name string (stored in PROGMEM-friendly literals)
 */
const char* getColorName(int hueDeg) {
  // Normalize to 0-359
  hueDeg = hueDeg % 360;

  if(hueDeg < 15)  return "Red";
  if(hueDeg < 30)  return "Red-Org";    // red-orange
  if(hueDeg < 45)  return "Orange";
  if(hueDeg < 70)  return "Yel-Org";    // yellow-orange
  if(hueDeg < 80)  return "Yellow";
  if(hueDeg < 100) return "Yel-Grn";    // yellow-green
  if(hueDeg < 140) return "Green";
  if(hueDeg < 170) return "Grn-Cyn";    // green-cyan
  if(hueDeg < 195) return "Cyan";
  if(hueDeg < 220) return "Sky Blue";
  if(hueDeg < 260) return "Blue";
  if(hueDeg < 290) return "Violet";
  if(hueDeg < 320) return "Magenta";
  if(hueDeg < 345) return "Rose";
  return "Red";                          // wraps back around
}
