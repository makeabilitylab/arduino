void setup() {
  pinMode(3, OUTPUT);
}

void loop() {

  for (int brightness = 0; brightness < 255; brightness++){
    // set the brightness of the LED pin
    analogWrite(3, brightness);

    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
}
