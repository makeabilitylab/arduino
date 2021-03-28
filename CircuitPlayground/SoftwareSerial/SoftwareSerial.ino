/*
 This example prints out data to both CPX serials: the USB serial and the TX/RX serial

 */

//https://forums.adafruit.com/viewtopic.php?f=24&t=117958
// #include <SoftwareSerial.h>
#include <Adafruit_CircuitPlayground.h> // Library ref: https://caternuson.github.io/Adafruit_CircuitPlayground/

//SoftwareSerial mySerial(A6, A7); // RX, TX

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.println("Goodnight moon!");

  Serial1.begin(9600);
  Serial1.println("Hello, world?");
}

void loop() // run over and over
{
  Serial.println("Main serial: Writing to serial with 'Serial.println'");
  Serial1.println("Secondary serial: Writing to serial with 'Serial1.println'");
  delay(500);
}
