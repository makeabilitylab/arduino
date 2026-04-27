
/**
 * Reads a single potentiometer to control the hue of an 8-LED WS2812B
 * NeoPixel stick (full brightness, full saturation), and displays the
 * raw ADC value, hue in degrees, NeoPixel HSV value, and an approximate
 * color name on an SSD1306 OLED display.
 *
 * Pot (A0): Controls hue (0-360°, mapped across the full color wheel)
 *
 * OLED layout (128x64):
 *   y=0   "HUE POT"                        (header, size 1)
 *   y=10  "Raw: <adc>"                     (size 1)
 *   y=20  "Hue: <deg>°"                    (size 1)
 *   y=30  "NeoPixel: <0-65535>"            (size 1)
 *   y=42  ── horizontal divider ──
 *   y=48  "> <ColorName>"                  (size 2, prominent)
 *
 * Wiring:
 *  - Pot wiper     → A0
 *  - NeoPixel DIN  → Pin 7 (avoid pin 2 on Leonardo; it's I2C SDA)
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
const int NEOPIXEL_PIN = 7; // Can't use PIN 2 on Leonardo because used by SDA
const int NUM_LEDS = 8;
Adafruit_NeoPixel _strip(NUM_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Potentiometer pin
const int HUE_POT_PIN = A0;
const int MAX_ANALOG_VAL = 1023;

// Named color regions on the HSV wheel (0-360 degrees)
// Each entry: { maxDegree, name }
// A hue falls into the first region whose maxDegree it is less than.
struct ColorRegion {
  int maxDeg;
  const char* name;
};

const ColorRegion COLOR_TABLE[] = {
  {  15, "Red"       },
  {  30, "Red-Orange"},
  {  45, "Orange"    },
  {  65, "Amber"     },
  {  80, "Yellow"    },
  { 100, "Yel-Green" },
  { 140, "Green"     },
  { 165, "Cyan-Green"},
  { 195, "Cyan"      },
  { 220, "Sky Blue"  },
  { 260, "Blue"      },
  { 285, "Violet"    },
  { 320, "Magenta"   },
  { 345, "Rose"      },
  { 361, "Red"       }   // wraps around
};

const int NUM_COLORS = sizeof(COLOR_TABLE) / sizeof(COLOR_TABLE[0]);

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
  _strip.setBrightness(15);
  _strip.show(); // Initialize all pixels to off
}

void loop()
{
  // Read potentiometer
  int huePotVal = analogRead(HUE_POT_PIN);

  // Map pot value to hue (0-65535 for NeoPixel HSV)
  uint16_t hue = map(huePotVal, 0, MAX_ANALOG_VAL, 0, 65535);

  // Hue in degrees (0-360) for display and color naming
  int hueDegrees = map(huePotVal, 0, MAX_ANALOG_VAL, 0, 360);

  // Set all LEDs to this hue at full saturation and brightness
  uint32_t color = _strip.gamma32(_strip.ColorHSV(hue, 255, 255));
  _strip.fill(color);
  _strip.show();

  // Draw the OLED display
  _display.clearDisplay();
  _display.setTextSize(1);

  // --- Row 0 (y=0): Header ---
  _display.setCursor(0, 0);
  _display.print(F("HUE POT"));

  // --- Row 1 (y=10): Raw ADC ---
  _display.setCursor(0, 10);
  _display.print(F("Raw input: "));
  _display.print(huePotVal);

  // --- Row 2 (y=20): Hue in degrees ---
  _display.setCursor(0, 20);
  _display.print(F("Hue: "));
  _display.print(hueDegrees);
  // Degree symbol (small circle) right after the number
  int16_t x1, y1;
  uint16_t tw, th;
  String degStr = "Hue: " + String(hueDegrees);
  _display.getTextBounds(degStr, 0, 0, &x1, &y1, &tw, &th);
  _display.drawCircle(tw + 2, 21, 1, SSD1306_WHITE);

  // --- Row 3 (y=30): NeoPixel HSV value ---
  _display.setCursor(0, 30);
  _display.print(F("NeoPixel: "));
  _display.print(hue);

  // --- Divider (y=42) ---
  _display.drawLine(0, 42, SCREEN_WIDTH - 1, 42, SSD1306_WHITE);

  // --- Row 5 (y=48): Named color, size 2 ---
  _display.setTextSize(2);
  const char* curColor = COLOR_TABLE[getColorIndex(hueDegrees)].name;
  _display.setCursor(0, 48);
  _display.print(curColor);

  _display.display();

  delay(50);
}

/**
 * Returns the index into COLOR_TABLE for a given hue in degrees (0-360).
 *
 * @param hueDeg  Hue in degrees (0-360)
 * @return        Index into COLOR_TABLE
 */
int getColorIndex(int hueDeg) {
  hueDeg = hueDeg % 360;
  for (int i = 0; i < NUM_COLORS; i++) {
    if (hueDeg < COLOR_TABLE[i].maxDeg) {
      return i;
    }
  }
  return NUM_COLORS - 1; // fallback to last (Red wrap)
}
