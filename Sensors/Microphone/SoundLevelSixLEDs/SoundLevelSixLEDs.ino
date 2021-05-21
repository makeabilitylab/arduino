/*
 * Simple VUMeter using an electret microphone (with built-in amp) on A0
 * and an LED for output on Pin 3
 * 
 * We also have a version with an OLED called: SoundLevelSixLEDsWithOLED.ino
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */


// 1024 with 5V, 676 with 3.3V on Arduino Uno or Leonardo
const int MIN_ANALOG_IN = 300;

const int MAX_ANALOG_OUT = 255;
const int MIC_INPUT_PIN = A0;


const int _micSampleWindow = 50; // Sample window width in ms (50 ms = 20Hz)

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
}

void loop() {  

  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  
  unsigned int signalMax = 0;
  unsigned int signalMin = MAX_ANALOG_IN;
  
  // collect data for 50 mS
  while (millis() - startMillis < _micSampleWindow){
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
}
