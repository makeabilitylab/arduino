/* 
 * This example receives a RED LED brightness value [0,1] and draws a circle corresponding
 * to that value
 *
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 */

import processing.serial.*;

// We communicate with the Arduino via the serial port
Serial _serialPort;

final int ARDUINO_SERIAL_PORT_INDEX = 7; // our serial port index

float _redLEDVal = 0.5; // between [0,1], received from serial

void setup(){
  //size(640, 480);
  fullScreen();
  
  // Open the serial port
  _serialPort = new Serial(this, Serial.list()[ARDUINO_SERIAL_PORT_INDEX], 9600);
}

void draw(){
  background(10); // set background color
  
  float maxCircleSize = min(width, height) * 0.9; // the circle should only grow to 90% of the screen
  float minCircleSize = 40;
  
  // calculate the color and circlesize based on the red LED value
  int redColor = (int)map(_redLEDVal, 0, 0.5, 0, 255);
  float circleSize = map(_redLEDVal, 0, 1, minCircleSize, maxCircleSize);
  
  // draw the circle
  fill(redColor, 0, 0); // set the color
  circle(width / 2f, height / 2f, circleSize);
  
  // add an outline
  stroke(255); // set the outline color to white
  noFill(); // turn off the 'fill' for this since we just want an outline
  circle(width / 2f, height / 2f, circleSize);
}

/**
* Called automatically when new data is received over the serial port. In this case,
* we use it for debugging as our debug Arduino Serial.print calls will send data over serial
*/
void serialEvent (Serial myPort) {
  try {
    // Grab the data off the serial port. See: 
    // https://processing.org/reference/libraries/serial/index.html
    String inString = trim(_serialPort.readStringUntil('\n'));
    
    if(inString != null){
      _redLEDVal = float(inString); // convert to a float
      println("Read in: " + inString + " LED val: " + _redLEDVal);
    }
  }
  catch(Exception e) {
    println(e);
  }
}
