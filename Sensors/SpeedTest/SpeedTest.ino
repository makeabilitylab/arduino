
// See: https://forum.arduino.cc/index.php?topic=6549.0

#define FASTADC 1

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


boolean _ledVal = true;
const int LED_OUTPUT_PIN = 2;

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);

  #if FASTADC
   // set prescale to 16
   sbi(ADCSRA,ADPS2) ;
   cbi(ADCSRA,ADPS1) ;
   cbi(ADCSRA,ADPS0) ;
  #endif

  // With FASTADC, achieved 44.7 KHz sampling frequency
//  while(1){
//    int analogVal = analogRead(A0);
//    digitalWrite(LED_OUTPUT_PIN, 0);
//    digitalWrite(LED_OUTPUT_PIN, 1);
//  }
}

void loop() {
  // put your main code here, to run repeatedly:
//  int analogVal = analogRead(A0);
  digitalWrite(LED_OUTPUT_PIN, _ledVal);
  _ledVal = !_ledVal;
  delay(10);
//  while(1){
//    int analogVal = analogRead(A0);
//    digitalWrite(LED_OUTPUT_PIN, 0);
//    digitalWrite(LED_OUTPUT_PIN, 1);
//  }
}



//#define FASTADC 1
//
//// defines for setting and clearing register bits
//#ifndef cbi
//#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
//#endif
//#ifndef sbi
//#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
//#endif
//
//void setup() {
// int start ;
// int i ;
// 
//#if FASTADC
// // set prescale to 16
// sbi(ADCSRA,ADPS2) ;
// cbi(ADCSRA,ADPS1) ;
// cbi(ADCSRA,ADPS0) ;
//#endif
//
// Serial.begin(9600) ;
// Serial.print("ADCTEST: ") ;
// start = millis() ;
// for (i = 0 ; i < 1000 ; i++)
//   analogRead(0) ;
// Serial.print(millis() - start) ;
// Serial.println(" msec (1000 calls)") ;
//}
//
//void loop() {
//}
