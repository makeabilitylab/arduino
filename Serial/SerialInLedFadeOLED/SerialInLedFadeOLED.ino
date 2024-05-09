/**
 * Reads in a text-encoded float over serial and sets an LED brightness
 * accordingly. Also prints the received value to the OLED and echos
 * the value back using Serial.println (for debugging)
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

const int DELAY_MS = 5;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int LED_OUTPUT_PIN = 5; // make sure to use a PWM pin
const int MAX_ANALOG_OUT = 255; // max PWM val

void setup() {
  Serial.begin(115200);

  pinMode(LED_OUTPUT_PIN, OUTPUT);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

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
    // Read data off the serial port until we get to the endline delimeter ('\n')
    // Store all of this data into a string
    String rcvdSerialData = Serial.readStringUntil('\n'); 

    // Convert the data into a float
    float valAsFloat = rcvdSerialData.toFloat();
    int pwmVal = valAsFloat * MAX_ANALOG_OUT;

    pwmVal = constrain(pwmVal, 0, MAX_ANALOG_OUT); // basic boundary checking
    
    analogWrite(LED_OUTPUT_PIN, pwmVal);
    displayOnOLED(valAsFloat);

    // Send this data back out on the serial port for debugging purposes
    Serial.print("Received: ");
    Serial.print(rcvdSerialData);
    Serial.print(" Converted to: ");
    Serial.print(valAsFloat, 4);
    Serial.print(" and ");
    Serial.println(pwmVal);
  }
  delay(DELAY_MS);
}

void displayOnOLED(float valAsFloat){
  int16_t x1, y1;
  uint16_t textWidth, textHeight;

  _display.setTextSize(3);
  //String valAsString = String(valAsFloat);
  _display.getTextBounds("0.0000", 0, 0, &x1, &y1, &textWidth, &textHeight);
  
  uint16_t yText = _display.height() / 2 - textHeight / 2;
  uint16_t xText = _display.width() / 2 - textWidth / 2;

  _display.clearDisplay();
  _display.setCursor(xText, yText);
  _display.println(valAsFloat, 4);
  _display.display(); 
}
