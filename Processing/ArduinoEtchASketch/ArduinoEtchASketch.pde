/* 
 * This example reads an x,y value from the serial port (where each value is [0,1])
 * and uses this to control an etch-a-sketch like drawing program
 * 
 * FUTURE IDEAS:
 *  - have the etch cursor smoke
 *  - show etch cursor (to do this, we'd have to draw off surface or store values in a array and then draw)
 *  - make stroke a gradient
 *   
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 */
import processing.serial.*;

// We communicate with the Arduino via the serial port
Serial _serialPort;

final int ARDUINO_SERIAL_PORT_INDEX = 7;

float _lastPtX = -1, _lastPtY = -1;
float _curPtX = -1, _curPtY = -1;

void setup() {
  
  size(640, 480);
  // fullScreen(); // uncomment this and comment out size to go fullscreen
  
  // Print all the available serial ports to the console
  printArray(Serial.list());

  // Open the serial port
  _serialPort = new Serial(this, Serial.list()[ARDUINO_SERIAL_PORT_INDEX], 9600);

  // Don't generate a serialEvent() unless you get a newline character:
  _serialPort.bufferUntil('\n');
  
  // paint the background only once
  background(10);
  
  // setup the etch-a-sketch pen
  stroke(255, 0, 255); // set the color //<>//
  strokeWeight(4); // sets the pen stroke width
}

void draw() {
  
  if(_curPtX != -1 && _lastPtX != -1){ // make sure these values have been initialized
    
    // Convert the [0,1] values to screen coordinate values
    float lastX = map(_lastPtX, 0, 1, 0, width);
    float lastY = map(_lastPtY, 0, 1, 0, height);
    
    float curX = map(_curPtX, 0, 1, 0, width);
    float curY = map(_curPtY, 0, 1, 0, height);
    
    // Draw the line
    line(lastX, lastY, curX, curY);
  }
}

void keyPressed(){
  if (key == ' ') { // if the user hits the spacebar, take a screenshot and save
    println("Saving current drawing!");
    saveFrame("etch-a-sketch-######.png");
  } else { // else if the user hits any other key, clear the drawing
    background(10);
  }
  
}

/**
 *
 *
 */
void serialEvent (Serial myPort) {

  try {
    // Grab the data off the serial port. See: 
    // https://processing.org/reference/libraries/serial/index.html
    String inString = trim(_serialPort.readStringUntil('\n'));
    
    // uncomment this to see what was read in
    // println("Read in: " + inString);

    // Split the comma separated data into individual values
    float [] data = float(split(inString, ','));
    
    if(data.length >= 2){ // we expect at least x,y values from serial    
      _lastPtX = _curPtX;
      _lastPtY = _curPtY;
      _curPtX = data[0];
      _curPtY = data[1]; 
    }

    // Print out the received data (this is just for debugging)
    // printArray(data);
  }
  catch(Exception e) {
    println(e);
  }
}
