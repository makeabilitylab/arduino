
/**
 * Control a servo motor angle with a potentiometer input and output
 * the angle on an OLED display. Uses exponential moving average (EMA)
 * smoothing to reduce ADC noise and only updates the servo when the
 * angle changes to eliminate jitter/buzz.
 * 
 * See the Arduino servo library:
 * https://www.arduino.cc/reference/en/libraries/servo/
 * 
 * This code is part of the Makeability Lab's Physical Computing textbook:
 * Servo lesson: https://makeabilitylab.github.io/physcomp/advancedio/servo.html
 * OLED lesson: https://makeabilitylab.github.io/physcomp/advancedio/oled.html
 * Smoothing input lesson (including EMA): https://makeabilitylab.github.io/physcomp/advancedio/smoothing-input.html  
 *
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */
#if defined(ESP32)
  // Install ESP32Servo library by Kevin Harrington via the 
  // Arduino Library Manager (search "ESP32Servo")
  #include <ESP32Servo.h>  
#else
  // Install Servo library by Michael Margolis via
  // the Arduino Library Manager (search "Servo")
  #include <Servo.h>
#endif

// Includes for OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int POTENTIOMETER_INPUT_PIN = A0;  

#if defined(ESP32)
  const int MAX_ANALOG_VAL = 4095;
  const int SERVO_OUTPUT_PIN = 13; // Safe pin on ESP32 Huzzah32
#else
  const int MAX_ANALOG_VAL = 1023;
  const int SERVO_OUTPUT_PIN = 9;
#endif

Servo _servo; 

// Exponential moving average (EMA) smoothing for the potentiometer input.
// Lower alpha = more smoothing (less responsive), higher = less smoothing.
// 0.15 is a good balance for a physical knob: damps ADC noise without
// introducing noticeable lag.
float _smoothedPotVal = 0;
const float ALPHA = 0.4;

// Track the last angle written to the servo so we only send updates
// when the value actually changes. This prevents the servo from
// continuously micro-correcting and buzzing.
int _lastServoAngle = -1;

void setup() 
{ 
  Serial.begin(9600);
  
  _servo.attach(SERVO_OUTPUT_PIN);  

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Seed the EMA with an initial reading so the smoothed value
  // starts at the current pot position rather than ramping up from 0
  _smoothedPotVal = analogRead(POTENTIOMETER_INPUT_PIN);

  _display.clearDisplay();      
  _display.setTextColor(SSD1306_WHITE); // Draw white text
  _display.setCursor(0, 0);     // Start at top-left corner
} 
 
void loop() 
{ 
  // Read the raw analog value (0-1023 for 10-bit ADC or 0-4095 for 12-bit ADC) 
  // from the potentiometer
  int potVal = analogRead(POTENTIOMETER_INPUT_PIN); 

  // Apply exponential moving average to smooth out ADC noise.
  // EMA formula: smoothed = alpha * newReading + (1 - alpha) * smoothed
  // This acts as a simple low-pass filter without needing a ring buffer.
  _smoothedPotVal = ALPHA * potVal + (1.0 - ALPHA) * _smoothedPotVal;
  int smoothedVal = (int)(_smoothedPotVal + 0.5); // Round to nearest int
  int servoAngle = map(smoothedVal, 0, MAX_ANALOG_VAL, 0, 180);

  // Only attach and write to the servo when the angle has actually changed.
  // This prevents servo buzz caused by repeatedly writing the same
  // (or nearly the same) position every loop iteration.
  // 
  // Tradeoffs: Detaching removes all holding torque, allowing the servo 
  // to be physically back-driven if under load. Additionally, re-attaching 
  // causes a brief physical twitch and current spike as the servo snaps back 
  // to the target angle.
  if(servoAngle != _lastServoAngle) {
    if(!_servo.attached()) {
      _servo.attach(SERVO_OUTPUT_PIN);
    }
    _servo.write(servoAngle); 
    _lastServoAngle = servoAngle;
  } else {
    _servo.detach();
  }

  // Begin OLED Display code...
  _display.clearDisplay(); 

  // Show raw A0 and smoothed value on the top line
  _display.setTextSize(1);
  _display.setCursor(0, 0);
  _display.print("A0:");
  _display.print(potVal);

  // Right-align the smoothed value on the same line
  String strSmoothed = String("A0 smooth:") + String(smoothedVal);
  int smoothedWidth = strSmoothed.length() * 6; // 6px per char at size 1
  _display.setCursor(SCREEN_WIDTH - smoothedWidth, 0);
  _display.print(strSmoothed);

  // Display the smoothed servo angle, centered on the screen
  _display.setTextSize(4);
  int16_t x1, y1;
  uint16_t textWidth, textHeight;
  String strAngle = (String)servoAngle;
  _display.getTextBounds(strAngle, 0, 0, &x1, &y1, &textWidth, &textHeight);
  uint16_t xText = _display.width() / 2 - textWidth / 2;
  uint16_t yText = _display.height() / 2 - textHeight / 2 + 6;
  _display.setCursor(xText, yText);
  _display.print(strAngle);

  // Draw a degree symbol as a small circle just to the upper-right
  // of the angle text. A circle renders much cleaner than the '°'
  // character at large text sizes on the SSD1306.
  int circleRadius = 3;
  int circleX = xText + textWidth + circleRadius + 2;
  int circleY = yText + circleRadius;
  _display.drawCircle(circleX, circleY, circleRadius, SSD1306_WHITE);

  _display.display();

  delay(10);
} 
