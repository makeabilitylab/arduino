/**
 *  Example program to demonstrate floating pin problem with digital input. 
 *  For more information, see: 
 *  https://makeabilitylab.github.io/physcomp/arduino/buttons.html#the-floating-pin-problem
 *
 *  By Jon E. Froehlich
 *  @jonfroehlich
 *  http://makeabilitylab.io
 *
 */
 
const int INPUT_BUTTON_PIN = 2;
const int OUTPUT_LED_PIN = 3;

void setup()
{
  pinMode(INPUT_BUTTON_PIN, INPUT);
  pinMode(OUTPUT_LED_PIN, OUTPUT);
  Serial.begin(9600); // for printing values to console
}

void loop()
{
  int buttonVal = digitalRead(INPUT_BUTTON_PIN); // returns 0 (LOW) or 1 (HIGH)
  if(buttonVal == HIGH){
    Serial.println("Button pressed!");
  }else{
    Serial.println("Button *NOT* pressed");
  }
  digitalWrite(OUTPUT_LED_PIN, buttonVal);

  delay(5);                                      // small delay
}
