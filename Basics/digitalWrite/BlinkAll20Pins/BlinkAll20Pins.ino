/*
 * Demonstrates how to use all 20 GPIO pins as digital output
 *
 * The Tinkercad circuit:
 * https://www.tinkercad.com/things/djhZYuYyqOR-using-all-20-gpio-pins-as-digital-out
 *
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 * More tutorials here: https://makeabilitylab.github.io/physcomp/
 * 
 */

const int MAX_NUM_PINS = 20;
const int DELAY_IN_MS = 200;

// Specify the order of the pins
const int _gpioPins[] = {13, 12, 11, 10, 9, 
                         8, 7, 6, 5, 4, 
                         3, 2, 1, 0, 19,
                         18, 17, 16, 15, 14};

void setup()
{ 
  // Loop through each pin and set as OUTPUT
  for(int i=0; i < MAX_NUM_PINS; i++){
    const int pinNum = _gpioPins[i];
  	pinMode(pinNum, OUTPUT); 
  }
}

void loop()
{
  // Loop through each pin in order and flash on/off
  for(int i=0; i < MAX_NUM_PINS; i++){
    const int pinNum = _gpioPins[i];
    digitalWrite(pinNum, HIGH);
  	delay(DELAY_IN_MS);
    digitalWrite(pinNum, LOW);
    delay(DELAY_IN_MS); 
  }
}