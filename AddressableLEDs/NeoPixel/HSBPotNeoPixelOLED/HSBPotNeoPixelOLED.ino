
/**
 * Reads three potentiometers to control the hue, saturation, and
 * brightness of an 8-LED WS2812B NeoPixel stick, and displays all
 * three values on an SSD1306 OLED display with bar graphs and an
 * approximate color name for the current hue.
 *
 * Pot 1 (A0): Controls hue (0-360°)
 * Pot 2 (A1): Controls saturation (0-255, displayed as 0-100%)
 * Pot 3 (A2): Controls brightness (0-255, displayed as 0-100%)
 *
 * Wiring:
 *  - Pot 1 wiper   → A0 (hue)
 *  - Pot 2 wiper   → A1 (saturation)
 *  - Pot 3 wiper   → A2 (brightness)
 *  - NeoPixel DIN  → Pin 2
 *  - NeoPixel VCC  → 5V
 *  - NeoPixel GND  → GND
 *  - OLED SDA      → A4
 *  - OLED SCL      → A5
 *
 * For our NeoPixel tutorial, see:
 * https://makeabilitylab.github.io/physcomp/advancedio/addressable-leds.html
 *
 * For our OLED tutorias, see:
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
const int SAT_POT_PIN = A1;
const int BRI_POT_PIN = A2;
const int MAX_ANALOG_VAL = 1023;

// Bar graph layout constants
const int BAR_X = 0;
const int BAR_WIDTH = 96;   // leaves room for value text to the right
const int BAR_HEIGHT = 7;

// Vertical layout: 3 sections, each 20px tall, with 1px dividers
// Section 0: y=0..19   (HUE)
// Divider:   y=20
// Section 1: y=22..41  (SAT)
// Divider:   y=42
// Section 2: y=44..63  (BRI)
const int SECTION_HEIGHT = 20;
const int SECTION_Y[] = { 0, 22, 44 };

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
  // Read potentiometers (with brief delays for ADC multiplexer settling)
  int huePotVal = analogRead(HUE_POT_PIN);
  delay(1);
  int satPotVal = analogRead(SAT_POT_PIN);
  delay(1);
  int briPotVal = analogRead(BRI_POT_PIN);

  // Map pot values to NeoPixel HSV ranges
  uint16_t hue = map(huePotVal, 0, MAX_ANALOG_VAL, 0, 65535);
  uint8_t saturation = map(satPotVal, 0, MAX_ANALOG_VAL, 0, 255);
  uint8_t brightness = map(briPotVal, 0, MAX_ANALOG_VAL, 0, 255);

  // Human-readable display values
  int hueDegrees = map(huePotVal, 0, MAX_ANALOG_VAL, 0, 360);
  int satPercent = map(satPotVal, 0, MAX_ANALOG_VAL, 0, 100);
  int briPercent = map(briPotVal, 0, MAX_ANALOG_VAL, 0, 100);

  // Set all LEDs to the same HSV color
  uint32_t color = _strip.gamma32(_strip.ColorHSV(hue, saturation, brightness));
  _strip.fill(color);
  _strip.show();

  // Draw the OLED display
  _display.clearDisplay();
  _display.setTextSize(1);

  // ========== HUE (section 0) ==========
  int y = SECTION_Y[0];

  _display.setCursor(0, y);
  _display.print(F("HUE"));

  // Color name right-aligned on the header row
  const char* colorName = getColorName(hueDegrees);
  drawRightAlignedText(colorName, y);

  // Bar graph + degree value beside it
  int hueBarFill = map(hueDegrees, 0, 360, 0, BAR_WIDTH - 2);
  drawBarGraph(BAR_X, y + 10, BAR_WIDTH, BAR_HEIGHT, hueBarFill);
  drawValueWithDegree(hueDegrees, BAR_X + BAR_WIDTH + 3, y + 10);

  // ========== SAT (section 1) ==========
  y = SECTION_Y[1];

  _display.setCursor(0, y);
  _display.print(F("SAT"));

  // Bar graph + percentage beside it
  int satBarFill = map(satPercent, 0, 100, 0, BAR_WIDTH - 2);
  drawBarGraph(BAR_X, y + 10, BAR_WIDTH, BAR_HEIGHT, satBarFill);

  String strSat = String(satPercent) + "%";
  _display.setCursor(BAR_X + BAR_WIDTH + 3, y + 10);
  _display.print(strSat);

  // ========== BRI (section 2) ==========
  y = SECTION_Y[2];

  _display.setCursor(0, y);
  _display.print(F("BRI"));

  // Bar graph + percentage beside it
  int briBarFill = map(briPercent, 0, 100, 0, BAR_WIDTH - 2);
  drawBarGraph(BAR_X, y + 10, BAR_WIDTH, BAR_HEIGHT, briBarFill);

  String strBri = String(briPercent) + "%";
  _display.setCursor(BAR_X + BAR_WIDTH + 3, y + 10);
  _display.print(strBri);

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
 * Draws a right-aligned string at the given y position.
 * Assumes text size is already set.
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
 * Draws an integer value followed by a small degree circle at the
 * given position. Uses drawCircle with radius 1 for a compact
 * superscript-style degree symbol.
 *
 * @param value   The integer value to display
 * @param x       The x position for the start of the text
 * @param y       The y position for the top of the text
 */
void drawValueWithDegree(int value, int x, int y) {
  String strVal = String(value);
  _display.setCursor(x, y);
  _display.print(strVal);

  // Measure where the text ends, then draw a tiny circle as the degree symbol
  int16_t x1, y1;
  uint16_t tw, th;
  _display.getTextBounds(strVal, 0, 0, &x1, &y1, &tw, &th);
  _display.drawCircle(x + tw + 2, y + 1, 1, SSD1306_WHITE);
}

/**
 * Returns an approximate color name for a given hue angle (0-360).
 * Uses 14 named regions spanning the HSV color wheel. Names are
 * kept short to fit on the OLED at text size 1.
 *
 * @param hueDeg  Hue in degrees (0-360)
 * @return        A short color name string
 */
const char* getColorName(int hueDeg) {
  hueDeg = hueDeg % 360;

  if(hueDeg < 15)  return "Red";
  if(hueDeg < 30)  return "Red-Org";
  if(hueDeg < 45)  return "Orange";
  if(hueDeg < 70)  return "Yel-Org";
  if(hueDeg < 80)  return "Yellow";
  if(hueDeg < 100) return "Yel-Grn";
  if(hueDeg < 140) return "Green";
  if(hueDeg < 170) return "Grn-Cyn";
  if(hueDeg < 195) return "Cyan";
  if(hueDeg < 220) return "Sky Blue";
  if(hueDeg < 260) return "Blue";
  if(hueDeg < 290) return "Violet";
  if(hueDeg < 320) return "Magenta";
  if(hueDeg < 345) return "Rose";
  return "Red";
}
