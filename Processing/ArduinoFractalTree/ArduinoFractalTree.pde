/* 
 * Creates a recursive fractal tree based on float values [0,1] from the serial port. 
 * The following interactions are supported:
 *  - Press spacebar to save a tree to a file 
 *  - Press the down arrow to have leaves drop from the tree (with animation)
 *
 * By Jon Froehlich
 * http://makeabilitylab.io
 *
 * This example is based on Jon Froehlich's Fractal Tree, which is based on The Coding Train 
 * Fractal Tree by Daniel Shiffman: https://www.youtube.com/watch?v=fcdNSZ9IzJM&t=2s. 
 */
import processing.serial.*;

// The serial port is necessary to read data in from the Arduino
// Think of this Processing program just like Arduino's Serial Monitor and Serial Plotter tools
Serial _serialPort;

final int ARDUINO_SERIAL_PORT_INDEX = 7; // our serial port index
final int MAX_TREE_SIZE = 10; // 11 branch connections

int _branchDepthCount = 0;
int _addLeavesAfterDepth = MAX_TREE_SIZE - 1;
int _desiredTreeDepth = 0;

boolean _leavesFall = false;
Branch _tree;

void setup() {
  size(640, 480);
  //fullScreen();
  
  // Open the serial port
  _serialPort = new Serial(this, Serial.list()[ARDUINO_SERIAL_PORT_INDEX], 9600);

  // Don't generate a serialEvent() unless you get a newline character:
  _serialPort.bufferUntil('\n');
  
  //create the tree! 
  createTree();
}

void createTree() {
  PVector a = new PVector(width / 2, height);
  PVector b = new PVector(width / 2, height - height/4.0);
  _tree = new Branch(null, a, b);
  _branchDepthCount = 0;
  _leavesFall = false;
}

void keyPressed() {
  if (key == CODED) {
    // If the user pressed the 'down' arrow, start the leaves falling animation
    if (keyCode == DOWN) {
      _leavesFall = true;
    }
  } else if (key == ' ') { // if the user hits the spacebar, take a screenshot and save
    println("Saving current tree!");
    saveFrame("tree-######.png");
  } else { // else if the user hits any other key, clear the tree
    createTree();
  }
}

void draw() {
  background(51); // draw the background

  // use the current mouse position to draw only part of the tree
  //int maxDepth = (int)map(mouseY, height, 0, 0, MAX_TREE_SIZE);
  _tree.draw(_desiredTreeDepth);

  // if true, animate leaves falling from tree
  if (_leavesFall) {
    _tree.leavesFall();
  }

  // println(frameRate);
}

void serialEvent (Serial myPort) {
  try {
    // Grab the data off the serial port. See: 
    // https://processing.org/reference/libraries/serial/index.html
    String inString = trim(_serialPort.readStringUntil('\n'));

    if (inString != null) {
      float curArduinoVal = -1;
      
      // Our parser can handle either csv strings or just one float per line
      if(inString.contains(",")){
        float [] data = float(split(inString, ','));
        curArduinoVal = data[0];
      }else{
        curArduinoVal = float(inString);
      }
       
      int newDesiredTreeDepth = (int)map(curArduinoVal, 0, 0.95, 0, MAX_TREE_SIZE);
      
      // TODO consider switching growth stuff to draw() function so we can better animate stuff
      // add a isGrowing recursive function to branch (just added this but needs testing)
      // only grow one depth at a time
      for(int i = _branchDepthCount; i <= newDesiredTreeDepth; i++){
        boolean addLeavesToBranch = _branchDepthCount >= _addLeavesAfterDepth;
        println("newDesiredTreeDepth=" + newDesiredTreeDepth + " _branchDepthCount=" + _branchDepthCount + 
                " _addLeavesAfterDepth=" + _addLeavesAfterDepth + " addLeavesToBranch=" + addLeavesToBranch +
                " MAX_TREE_SIZE=" + MAX_TREE_SIZE);
        _tree.addBranchesToEnd(addLeavesToBranch);
        _branchDepthCount++;
      }
      _desiredTreeDepth = newDesiredTreeDepth;
      
      //println("Read in: " + inString + " Converted val: " + curArduinoVal + "(" + (int)(curArduinoVal * 1023) + ")");
      //println("desiredTreeDepth=" + _desiredTreeDepth + " _branchDepthCount=" + _branchDepthCount 
      //  + " MAX_TREE_SIZE=" + MAX_TREE_SIZE + " _addLeavesAfterDepth=" + _addLeavesAfterDepth);
    }
  }
  catch(Exception e) {
    println(e);
  }
}
