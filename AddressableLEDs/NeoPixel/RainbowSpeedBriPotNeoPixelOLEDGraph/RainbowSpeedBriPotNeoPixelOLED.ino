
/**
 * Animates a cycling rainbow across an 8-LED WS2812B NeoPixel stick.
 * Two potentiometers control the animation speed and LED brightness.
 * An SSD1306 OLED display shows both values with bar graphs.
 *
 * The rainbow bounces back and forth through the hue wheel rather
 * than wrapping, matching the behavior of RainbowAnimationBidirectional.
 *
 * Pot 1 (A0): Controls animation speed (slow to fast)
 * Pot 2 (A1): Controls brightness (0-100%)
 *
 * The bottom section shows a bouncing position bar that tracks the
 * current hue offset as it sweeps through the color wheel, giving
 * a visual sense of the animation cycle on the monochrome display.
 *
 * Wiring:
 *  - Pot 1 wiper   → A0 (speed)
 *  - Pot 2 wiper   → A1 (brightness)
 *  - NeoPixel DIN  → Pin 7 (avoid pin 2 on Leonardo; it's I2C SDA)
 *  - NeoPixel VCC  → 5V
 *  - NeoPixel GND  → GND
 *  - OLED SDA      → SDA (pin 2 on Leonardo, A4 on Uno)
 *  - OLED SCL      → SCL (pin 3 on Leonardo, A5 on Uno)
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
const int NEOPIXEL_PIN = 7;  // Avoid pin 2 on Leonardo (I2C SDA)
const int NUM_LEDS = 8;
const uint32_t MAX_HUE = 65536; // Full circle (360°) in 16-bit hue
Adafruit_NeoPixel _strip(NUM_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Potentiometer pins
const int SPEED_POT_PIN = A0;
const int BRI_POT_PIN = A1;
const int MAX_ANALOG_VAL = 1023;

// Animation state
int32_t _firstPixelHue = 0;
int8_t _direction = 1;  // 1 = forward, -1 = reverse

// Speed range: pot maps to a hue step per frame (1 = crawl, 2048 = fast)
const int MIN_HUE_STEP = 1;
const int MAX_HUE_STEP = 2048;

// Bar graph layout constants
const int BAR_X = 0;
const int BAR_WIDTH = 96;   // leaves room for value text to the right
const int BAR_HEIGHT = 7;

// Vertical layout: 2 data sections (20px each) + dividers + position bar
// Section 0: y=0..19   (SPD)
// Divider:   y=20
// Section 1: y=22..41  (BRI)
// Divider:   y=42
// Section 2: y=44..63  (hue position indicator)
const int SECTION_Y[] = { 0, 22, 44 };

// Position indicator bar
const int POS_BAR_Y = 50;
const int POS_BAR_HEIGHT = 10;

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
  _strip.show(); // Initialize all pixels to off
}

void loop()
{
  // Read potentiometers (with brief delay for ADC multiplexer settling)
  int speedPotVal = analogRead(SPEED_POT_PIN);
  delay(1);
  int briPotVal = analogRead(BRI_POT_PIN);

  // Map pot values
  int hueStep = map(speedPotVal, 0, MAX_ANALOG_VAL, MIN_HUE_STEP, MAX_HUE_STEP);
  uint8_t brightness = map(briPotVal, 0, MAX_ANALOG_VAL, 0, 255);
  int briPercent = map(briPotVal, 0, MAX_ANALOG_VAL, 0, 100);

  // Advance the rainbow animation (bounce at boundaries)
  _firstPixelHue += hueStep * _direction;

  if(_firstPixelHue >= (int32_t)MAX_HUE) {
    _firstPixelHue = MAX_HUE;
    _direction = -1;
  } else if(_firstPixelHue <= 0) {
    _firstPixelHue = 0;
    _direction = 1;
  }

  // Update NeoPixels: spread the rainbow across all LEDs
  _strip.setBrightness(brightness);
  for(int i = 0; i < NUM_LEDS; i++) {
    uint32_t pixelHue = _firstPixelHue + (i * MAX_HUE / NUM_LEDS);
    _strip.setPixelColor(i, _strip.gamma32(_strip.ColorHSV(pixelHue)));
  }
  _strip.show();

  // ========== OLED DISPLAY ==========
  _display.clearDisplay();
  _display.setTextSize(1);

  // ========== SPD (section 0, y=0..19) ==========
  int y = SECTION_Y[0];
  _display.setCursor(0, y);
  _display.print(F("SPD"));

  // Speed label on the header row
  drawRightAlignedText(hueStep < 512 ? "Slow" : (hueStep < 1400 ? "Med" : "Fast"), y);

  // Bar graph + numeric step value beside it
  int speedBarFill = map(hueStep, MIN_HUE_STEP, MAX_HUE_STEP, 0, BAR_WIDTH - 2);
  drawBarGraph(BAR_X, y + 10, BAR_WIDTH, BAR_HEIGHT, speedBarFill);
  _display.setCursor(BAR_X + BAR_WIDTH + 3, y + 10);
  _display.print(hueStep);

  // ========== DIVIDER ==========
  _display.drawFastHLine(0, 20, SCREEN_WIDTH, SSD1306_WHITE);

  // ========== BRI (section 1, y=22..41) ==========
  y = SECTION_Y[1];
  _display.setCursor(0, y);
  _display.print(F("BRI"));

  // Bar graph + percentage beside it
  int briBarFill = map(briPercent, 0, 100, 0, BAR_WIDTH - 2);
  drawBarGraph(BAR_X, y + 10, BAR_WIDTH, BAR_HEIGHT, briBarFill);
  _display.setCursor(BAR_X + BAR_WIDTH + 3, y + 10);
  _display.print(briPercent);
  _display.print(F("%"));

  // ========== DIVIDER ==========
  _display.drawFastHLine(0, 42, SCREEN_WIDTH, SSD1306_WHITE);

  // ========== HUE POSITION INDICATOR (section 2, y=44..63) ==========
  y = SECTION_Y[2];
  _display.setCursor(0, y);
  _display.print(F("HUE POS"));

  // Show bounce direction with a small arrow
  drawRightAlignedText(_direction > 0 ? ">>>" : "<<<", y);

  // Draw a position bar showing where we are in the 0-65535 hue cycle.
  // The filled region represents the rainbow "window" that the 8 LEDs
  // span, bouncing back and forth across the full bar.
  _display.drawRect(0, POS_BAR_Y, SCREEN_WIDTH, POS_BAR_HEIGHT, SSD1306_WHITE);

  // Cursor position: where the first pixel's hue falls in the cycle
  // Reversed so the indicator matches the visual direction on the strip
  int cursorX = map(_firstPixelHue, 0, MAX_HUE - 1, SCREEN_WIDTH - 3, 1);
  int windowWidth = SCREEN_WIDTH / 8;  // the 8 LEDs span 1/8 of the wheel

  // Clamp the window to stay within the bar
  if(cursorX + windowWidth > SCREEN_WIDTH - 1) {
    cursorX = SCREEN_WIDTH - 1 - windowWidth;
  }
  if(cursorX < 1) {
    cursorX = 1;
  }

  _display.fillRect(cursorX, POS_BAR_Y + 1, windowWidth, POS_BAR_HEIGHT - 2, SSD1306_WHITE);

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
