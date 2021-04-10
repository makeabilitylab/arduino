/**
 * "Explodes" images into 3D space using float value parsed from the serial port (range [0,1])
 * and the brightness of the image.
 * 
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 * Based on heavily on Examples/Image Processing/Explode by Daniel Shiffman
 *
 * FUTURE IDEAS:
 * - Enumerate through different images in photos dir on mouse click
 */

import processing.serial.*;

// The serial port is necessary to read data in from the Arduino
// Think of this Processing program just like Arduino's Serial Monitor and Serial Plotter tools
Serial _serialPort;

final int ARDUINO_SERIAL_PORT_INDEX = 7; // our serial port index

PImage img;       // The source image
int cellsize = 2; // Dimensions of each cell in the grid
int columns, rows;   // Number of columns and rows in our system
float _curArduinoVal;

void setup() {
  size(800, 600, P3D); 
  img = loadImage("photos/llama.jpg");  // Load the image
  columns = img.width / cellsize;  // Calculate # of columns
  rows = img.height / cellsize;  // Calculate # of rows
  
  // Open the serial port
  _serialPort = new Serial(this, Serial.list()[ARDUINO_SERIAL_PORT_INDEX], 9600);

  // Don't generate a serialEvent() unless you get a newline character:
  _serialPort.bufferUntil('\n');
}

void draw() {
  background(0);

  int xDiff = width - img.width;
  int yDiff = height - img.height;
  float explosionFactor = _curArduinoVal;
  //float explosionFactor = (mouseX / float(width));
      
  // Begin loop for columns
  for ( int i = 0; i < columns; i++) {
    // Begin loop for rows
    for ( int j = 0; j < rows; j++) {
      int x = i*cellsize + cellsize/2;  // x position
      int y = j*cellsize + cellsize/2;  // y position
      int loc = x + y*img.width;  // Pixel array location
      color c = img.pixels[loc];  // Grab the color
      
      // Calculate a z position as a function of Arduino input and pixel brightness
      float z = explosionFactor * brightness(img.pixels[loc]) - 20.0;
      
      // Translate to the location, set fill and stroke, and draw the rect
      pushMatrix();
      translate(x + xDiff/2.0, y + yDiff/2.0, z);
      fill(c, 204);
      noStroke();
      rectMode(CENTER);
      rect(0, 0, cellsize, cellsize);
      popMatrix();
    }
  }
}

void serialEvent (Serial myPort) {
  try {
    // Grab the data off the serial port. See: 
    // https://processing.org/reference/libraries/serial/index.html
    String inString = trim(_serialPort.readStringUntil('\n'));

    if (inString != null) {

      // Our parser can handle either csv strings or just one float per line
      if (inString.contains(",")) {
        float [] data = float(split(inString, ','));
        _curArduinoVal = data[0];
      } else {
        _curArduinoVal = float(inString);
      }
    }
  }
  catch(Exception e) {
    println(e);
  }
}
