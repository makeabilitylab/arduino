/*
 * Simple VUMeter using an electret microphone (with built-in amp) on A0 and for
 * output, we use the PWM pins with LEDs and the OLED display.
 * 
 * We also have a version without the OLED called: SoundLevelSixLEDs.ino
 * 
 * Based on
 * https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
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

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 1024 with 5V, 676 with 3.3V on Arduino Uno or Leonardo
const int MAX_ANALOG_IN = 600; 

const int MAX_ANALOG_OUT = 255;
const int MIC_INPUT_PIN = A0;


const int MIN_SHAPE_SIZE = 0;
int _maxShapeSize = 0;

const int sampleWindow = 50; // Sample window width in mS (50 ms = 20Hz)

const int GREEN_LED1_OUTPUT_PIN = 5;
const int GREEN_LED2_OUTPUT_PIN = 6;
const int YELLOW_LED1_OUTPUT_PIN =9;
const int YELLOW_LED2_OUTPUT_PIN = 10;
const int RED_LED1_OUTPUT_PIN = 11;
const int RED_LED2_OUTPUT_PIN = 13;
const int NUM_LEDS = 6;
const int LED_PINS[NUM_LEDS] = {
  GREEN_LED1_OUTPUT_PIN,
  GREEN_LED2_OUTPUT_PIN,
  YELLOW_LED1_OUTPUT_PIN,
  YELLOW_LED2_OUTPUT_PIN,
  RED_LED1_OUTPUT_PIN,
  RED_LED2_OUTPUT_PIN,
};

// for tracking fps
float _fps = 0;
unsigned long _frameCount = 0;
unsigned long _fpsStartTimeStamp = 0;

void setup() {
  Serial.begin(9600);
  for(int i = 0; i < NUM_LEDS; i++){
    int ledPin = LED_PINS[i];
    pinMode(ledPin, OUTPUT);
  }
  pinMode(MIC_INPUT_PIN, INPUT);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  _maxShapeSize = min(_display.width(), _display.height());

  _display.clearDisplay();
  _display.setTextSize(1);      // Normal 1:1 pixel scale
  _display.setTextColor(SSD1306_WHITE); // Draw white text
  _display.setCursor(0, 0);     // Start at top-left corner
  _display.display();

  _fpsStartTimeStamp = millis();
}

void loop() {  

  
  unsigned int peakToPeak = 0;   // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = MAX_ANALOG_IN;
  
  // collect data for 50 ms
  unsigned long startMillis = millis();  // Start of sample window
  while (millis() - startMillis < sampleWindow){
    const int micInVal = analogRead(MIC_INPUT_PIN);
    
    if (micInVal > signalMax){
      signalMax = micInVal;  // save just the max levels
    }else if (micInVal < signalMin){
      signalMin = micInVal;  // save just the min levels
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  peakToPeak = constrain(peakToPeak, 0, MAX_ANALOG_IN);

  // Translate peakToPeak voltage signal to a VU meter value
  long vuMeterVal = map(peakToPeak, 0, MAX_ANALOG_IN, 0, MAX_ANALOG_OUT * NUM_LEDS);

  // Figure out how many LEDs should be fully on + remainder for analog out
  int numLedsOn = vuMeterVal / MAX_ANALOG_OUT;
  int lastLedVal = vuMeterVal - (numLedsOn * MAX_ANALOG_OUT);

  // Set LED output vals
  for(int i = 0; i < NUM_LEDS; i++){
    int ledPin = LED_PINS[i];
    
    if(i < numLedsOn){
      digitalWrite(ledPin, HIGH);
    }else if(i == numLedsOn){
      analogWrite(ledPin, lastLedVal);
    }else{
      digitalWrite(ledPin, LOW);
    }
  }

  // Draw OLED circle  
  _display.clearDisplay();
    
  int shapeSize = map(peakToPeak, 0, MAX_ANALOG_IN, MIN_SHAPE_SIZE, _maxShapeSize);
  int radius = shapeSize / 2;
  int xCenter = _display.width() / 2;
  int yCenter = _display.height() / 2; 
  _display.fillCircle(xCenter, yCenter, radius, SSD1306_WHITE);

  // Draw sound level
  _display.setCursor(0, 0);
  _display.print("Sound:");
  _display.println(peakToPeak);

  // Draw frame count
  int16_t x1, y1;
  uint16_t w, h;
  _display.getTextBounds("XX.X fps", 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() - w, 0);
  _display.print(_fps, 1);
  _display.print(" fps");

  _display.display();

  calcFrameRate();
}

/**
 * Call this every frame to calculate frame rate
 */
void calcFrameRate() {
    
  unsigned long elapsedTime = millis() - _fpsStartTimeStamp;
  _frameCount++;
  if (elapsedTime > 1000) {
    _fps = _frameCount / (elapsedTime / 1000.0);
    _fpsStartTimeStamp = millis();
    _frameCount = 0;
  }
}
