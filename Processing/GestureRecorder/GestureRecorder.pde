/** //<>//
 * Records accelerometer-based gestures. Works with the Arduino program
 * ADXL335SerialWriter.ino, LISDHSerialWriter.ino or any program that 
 * provides a CSV input stream on the serial port of "timestamp, x, y, z"
 *
 * Program assumes integer values coming over Serial
 *   
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * TODO:
 * - [done] Hit space to start and stop. When starting, counts down from 3, 2, 1
 * - [done] Support hardware button press to start
 * - [done] Status is shown in big letters
 * - [done] Saves both full sensor stream plus presegmented?
 * - [done] Maybe save Arduino time, Processing timestamp too?
 * - [done] Draw axes? 
 * - [done] Write out save file name after gesture finished recording? Maybe put in GestureAnnotation overlay
 * - [done] When the window is closed (or closing), flush the printwriter and close it (to ensure all of the fulldata.csv is saved)?
 *
 * Future ideas:
 * - Shows capture snapshot?
 * - GUI to select which gesture to record
 * - Print out which mode we are using for comm (BLUETOOTH, Serial, WiFi)
 * 
 */

import processing.serial.*;
import java.awt.Rectangle;
import java.io.BufferedWriter;
import java.io.FileWriter;
import processing.net.*;

final String GESTURE_DIR_NAME = "Gestures";
final String FULL_DATASTREAM_RECORDING_FILENAME = "fulldatastream.csv";
final boolean _createNewFullDatastreamFileOnEveryExecution = false;
String _fullDataStreamFilenameWithPath;

final color GRID_COLOR = color(160, 160, 160);
final color XCOLOR = color(255, 61, 0, 200);
final color YCOLOR = color(73, 164, 239, 200);
final color ZCOLOR = color(255, 183, 0, 200);
final color [] SENSOR_VALUE_COLORS = { XCOLOR, YCOLOR, ZCOLOR };
final color DEFAULT_BACKGROUND_COLOR = color(44, 42, 41);
final int DISPLAY_TIMEWINDOW_MS = 1000 * 20; // 20 secs. You can change this to view more/less data

// Make sure to change this! If you're not sure what port your Arduino is using
// Run this Processing sketch and look in the console, then change the number accordingly

final String ARDUINO_SERIAL_PORT_NAME = "COM3"; // CHANGE THIS TO APPROPRIATE PORT! 
// WIFI SETTINGS
// This should be false even in bluetooth mode since that just emulates a com port
final boolean ARDUINO_WIFI_MODE = false; // CHANGE THIS TO TRUE IF AND ONLY IF YOU ARE USING WIFI MODE
final int ARDUINO_WIFI_PORT = 10002; // THIS IS THE PORT THAT THEY WILL TRY TO COMMUNICATE OVER


// Set the baud rate to same as Arduino (e.g., 9600 or 115200)
final int SERIAL_BAUD_RATE = 115200; // CHANGE THIS TO MATCH BAUD RATE ON ARDUINO

// Data buffer shared between the event thread and UI thread. Must use synchronized
// to access and manipulate
ArrayList<AccelSensorData> _sensorBuffer = new ArrayList<AccelSensorData>();

// Buffer to display values to the screen
ArrayList<AccelSensorData> _displaySensorData = new ArrayList<AccelSensorData>();

// Writes accelerometer data to the file system. Because this PrintWriter is shared
// between the event thread and UI thread, must use synchronized to access
PrintWriter _printWriterAllData;

// The serial port is necessary to read data in from the Arduino
Serial _serialPort;

// Wifi server
Server _wifiServer;

long _currentXMin; // the far left x-axis value on the graph
Rectangle _legendRect; // location and drawing area of the legend
boolean _dynamicYAxis = true;
int _minSensorVal = 0;
int _maxSensorVal = 1023;
long _serialLinesRcvd = 0; // for coarse grain sampling rate calc
long _firstSerialValRcvdTimestamp = -1;

// Gesture recording stuff
final color RECORDING_BACKGROUND_COLOR = color(80, 0, 0);
final int NUM_SAMPLES_TO_RECORD_PER_GESTURE = 5;

// Feel free to rename "Custom" to something that is more semantically meaningful describing your custom gesture
// For example, I did "Bunny Hops"
final String [] GESTURES = { "Backhand Tennis", "Forehand Tennis", "Underhand Bowling", 
  "Baseball Throw", "Midair Clockwise 'O'", "At Rest", "Midair Counter-clockwise 'O'", 
  "Midair Zorro 'Z'", "Midair 'S'", "Shake", "Custom" };
int _curGestureIndex = 0;
HashMap<String, Integer> _mapGestureNameToRecordedCount = new HashMap<String, Integer>(); // tracks recorded gesture counts
ArrayList<GestureRecording> _gestureRecordings = new ArrayList<GestureRecording>(); // sensor data to dump to file

final int COUNTDOWN_TIME_MS = 4 * 1000; // how long to show a countdown timer before recording a gesture
long _timestampStartCountdownMs = -1; // timestamp of when the countdown timer was started
boolean _recordingGesture = false; // true if we are currently recording a gesture, false otherwise
long _timestampSinceLastGestureRecordToggle = -1;
final int MIN_TIME_BETWEEN_GESTURE_TOGGLE_MS = 500;

void setup() {
  size(1024, 576);

  String filenameNoPath = FULL_DATASTREAM_RECORDING_FILENAME;

  if (_createNewFullDatastreamFileOnEveryExecution) {
    String filenameWithoutExt = filenameNoPath.substring(0, filenameNoPath.length()-4);
    String filenameExt = filenameNoPath.substring(filenameNoPath.length()-4, filenameNoPath.length());
    filenameNoPath = filenameWithoutExt + System.currentTimeMillis() + filenameExt;
  }

  File fDir = new File(sketchPath(GESTURE_DIR_NAME));
  if (!fDir.exists()) {
    fDir.mkdirs();
  }
  File file = new File(fDir, filenameNoPath); 

  _fullDataStreamFilenameWithPath = file.getAbsolutePath();
  println("Saving accel data to: " + _fullDataStreamFilenameWithPath);

  try {
    // We save all incoming sensor data to a file (by appending)
    // Appending text to a file: 
    //  - https://stackoverflow.com/questions/17010222/how-do-i-append-text-to-a-csv-txt-file-in-processing
    //  - https://docs.oracle.com/javase/7/docs/api/java/io/FileWriter.html
    //  - Use sketchPath(string) to store in local sketch folder: https://stackoverflow.com/a/36531925
    _printWriterAllData = new PrintWriter(new BufferedWriter(new FileWriter(file, true)));
  }
  catch (IOException e) {
    e.printStackTrace();
  }

  if (ARDUINO_WIFI_MODE) {
    // https://www.processing.org/reference/libraries/net/Server.html
    _wifiServer = new Server(this, ARDUINO_WIFI_PORT);
    print("Starting server");
    while (!_wifiServer.active()) {
      print(".");
      delay(50);
    }
    println("\nWe have started a server at "+Server.ip());
    thread("processServer"); // start a thread that reads for the server
  } else {

    // Print to console all the available serial ports
    String [] serialPorts = getAndPrintSerialPortInfo();

    if (serialPorts.length <= 0) {
      println("You appear to have *ZERO* active serial ports. Make sure your Arduino is plugged in. Exiting...");
      exit();
      return;
    }

    int ARDUINO_SERIAL_PORT_INDEX = getIndexOfArduinoSerialPort(ARDUINO_SERIAL_PORT_NAME);

    if (ARDUINO_SERIAL_PORT_INDEX == -1) {
      println("Could not find the serial port: " + ARDUINO_SERIAL_PORT_NAME + ". Exiting...");
      exit();
      return;
    }

    // Open the serial port
    try {
      println("Attempting to initialize the serial port " + ARDUINO_SERIAL_PORT_NAME + " with baud rate = " + SERIAL_BAUD_RATE);
      println("This index corresponds to serial port at index: " + ARDUINO_SERIAL_PORT_INDEX);
      _serialPort = new Serial(this, serialPorts[ARDUINO_SERIAL_PORT_INDEX], SERIAL_BAUD_RATE);

      // We need to clear the port (or sometimes there is leftover data)
      // (Yes, this is strange, but once we implemented this clear,
      // we were no longer seeing garbage data in the beginning of our printwriter stream)
      _serialPort.clear();
    }
    catch(Exception e) {
      println("Serial port exception: " + e);
      e.printStackTrace();
      exit();
      return;
    }

    if (_serialPort == null) {
      println("Could not initialize the serial port at " + ARDUINO_SERIAL_PORT_INDEX + ". Exiting...");
      exit();
      return;
    }

    // Don't generate a serialEvent() unless you get a newline character:
    _serialPort.bufferUntil('\n');
  }

  _currentXMin = System.currentTimeMillis() - DISPLAY_TIMEWINDOW_MS;

  int legendHeight = 60;
  int legendWidth = 200;
  int legendXBuffer = 60;
  int legendYBuffer = 5;

  //_legendRect = new Rectangle(width - legendWidth - legendXBuffer, legendYBuffer, legendWidth, legendHeight); // legend at top-right
  _legendRect = new Rectangle(legendXBuffer, legendYBuffer, legendWidth, legendHeight); // legend at top-left

  println("Waiting for Serial data...");
  // println(Thread.currentThread());
}

void draw() {

  if (_recordingGesture) {
    background(RECORDING_BACKGROUND_COLOR);
  } else {
    background(DEFAULT_BACKGROUND_COLOR);
  }

  // Check for new sensor data
  ArrayList<AccelSensorData> newData = null;
  synchronized(_sensorBuffer) {
    newData = new ArrayList<AccelSensorData>(_sensorBuffer);
    _sensorBuffer.clear();
  }

  // Dump new data into _displaySensorData and curGestureRecording
  for (int i = 0; i < newData.size(); i++) {
    AccelSensorData accelSensorData = newData.get(i);
    checkAndSetNewMinMaxSensorValues(accelSensorData);
    _displaySensorData.add(accelSensorData);

    if (_recordingGesture) {
      GestureRecording curGestureRecording = _gestureRecordings.get(_gestureRecordings.size() - 1);
      curGestureRecording.listSensorData.add(accelSensorData);
    }
  }  

  // Remove data that is no longer relevant to be displayed
  while (_displaySensorData.size() > 0 && 
    _displaySensorData.get(0).timestamp < _currentXMin) {
    _displaySensorData.remove(0);
  }

  drawYAxis();

  //println("Drawing! _displaySensorData.size()=" + _displaySensorData.size() + " _timeWindowMs=" + _timeWindowMs);
  for (int i = 1; i < _displaySensorData.size(); i++) {
    AccelSensorData lastAccelSensorData = _displaySensorData.get(i - 1);
    AccelSensorData curAccelSensorData = _displaySensorData.get(i);

    drawSensorLine(XCOLOR, lastAccelSensorData.timestamp, lastAccelSensorData.x, curAccelSensorData.timestamp, curAccelSensorData.x);
    drawSensorLine(YCOLOR, lastAccelSensorData.timestamp, lastAccelSensorData.y, curAccelSensorData.timestamp, curAccelSensorData.y);
    drawSensorLine(ZCOLOR, lastAccelSensorData.timestamp, lastAccelSensorData.z, curAccelSensorData.timestamp, curAccelSensorData.z);
  }

  long curTimestampMs = System.currentTimeMillis();
  long elapsedTimeMs = curTimestampMs - _timestampStartCountdownMs;
  int countdownTimeSecs = (int)((COUNTDOWN_TIME_MS - elapsedTimeMs) / 1000.0);

  if (_timestampStartCountdownMs != -1 && countdownTimeSecs <= 0 && !_recordingGesture) {
    _recordingGesture = true;
    GestureRecording gestureRecording = new GestureRecording(GESTURE_DIR_NAME, GESTURES[_curGestureIndex], curTimestampMs);
    _gestureRecordings.add(gestureRecording);
  }

  drawInstructions(countdownTimeSecs); 
  drawGestureRecordingAnnotations();
  drawLegend(_legendRect);
  drawDebugInfo();
  drawRecordingStatus();
}

/**
 * Writes out recording status to the screen
 */
void drawRecordingStatus() {
  textSize(10);
  float strHeight = textAscent() + textDescent();
  float yText = height - (strHeight * GESTURES.length) - 3;
  noStroke();

  for (int i = 0; i < GESTURES.length; i++) {
    int sampleNum = min(getNumGesturesRecordedWithName(GESTURES[i]) + 1, NUM_SAMPLES_TO_RECORD_PER_GESTURE);
    String str = GESTURES[i] + " (" + sampleNum + "/" + NUM_SAMPLES_TO_RECORD_PER_GESTURE + ")";
    float strWidth = textWidth(str) + 10;

    if (_curGestureIndex == i) {
      fill(0, 240, 0, 230);
    } else {
      fill(255, 255, 255, 200);
    }

    text(str, width- strWidth, yText);
    yText += strHeight;
  }
}

/**
 * Writes out basic instructions for the user
 */
void drawInstructions(int countdownTimeSecs) {


  String strInstructions = "";
  textSize(30);
  noStroke();
  fill(255, 255, 255, 200);
  if (_displaySensorData.size() <= 0) {
    textSize(50);
    strInstructions = "Waiting for Serial data...";
  } else if (_timestampStartCountdownMs != -1) {
    // if we're here, the user hit the spacebar and we're either ready to record or recording
    // draw center of screen
    String str = ""; 
    if (_recordingGesture) {
      float strHeight = textAscent() + textDescent();
      int sampleNum = getNumGesturesRecordedWithName(GESTURES[_curGestureIndex]) + 1;
      //str = "Recording Sample " + sampleNum + "/" + NUM_SAMPLES_TO_RECORD_PER_GESTURE + " of '" + GESTURES[_curGestureIndex]  + "'!";
      str = "Now recording '" + GESTURES[_curGestureIndex]  + "' (" + sampleNum + "/" + NUM_SAMPLES_TO_RECORD_PER_GESTURE + ")";
      float strWidth = textWidth(str);
      float yText = height / 4.0 + strHeight / 2.0 - textDescent();
      text(str, width / 2.0 - strWidth / 2.0, yText);

      textSize(20);
      str = "Hit SPACEBAR or BUTTON to stop recording.";
      strWidth = textWidth(str);

      yText += strHeight + 2;

      text(str, width / 2.0 - strWidth / 2.0, yText);
    } else {
      // counting down
      textSize(35);
      str = "Recording '" + GESTURES[_curGestureIndex]  + "' in...";
      float strWidth = textWidth(str);
      float strHeight = textAscent() + textDescent();
      text(str, width / 2.0 - strWidth / 2.0, height / 4.0 + strHeight / 2.0 - textDescent());

      textSize(80);
      str = String.format("%d", countdownTimeSecs);
      strHeight = textAscent() + textDescent();
      strWidth = textWidth(str);
      text(str, width / 2.0 - strWidth / 2.0, height / 2.0 + strHeight / 2.0 - textDescent());
    }
  } else if (_curGestureIndex < GESTURES.length) {
    textSize(30);
    fill(255, 255, 255);
    int sampleNum = getNumGesturesRecordedWithName(GESTURES[_curGestureIndex]) + 1;
    strInstructions = "Hit SPACEBAR or BUTTON to record sample " + sampleNum + "/" + NUM_SAMPLES_TO_RECORD_PER_GESTURE 
      + " of gesture:\n'" + GESTURES[_curGestureIndex] + "'";
  } else {
    strInstructions = "You did it! Gesture recording completed!";
  }

  if (strInstructions.length() > 0) {
    float strWidth = textWidth(strInstructions);
    float strHeight = textAscent() + textDescent();

    text(strInstructions, width / 2.0 - strWidth / 2.0, height / 4.0 + strHeight / 2.0 - textDescent());
  }
}

/**
 * Called automatically when a key is pressed. We use this to capture the spacebar keypress (used to start/stop)
 */
void keyPressed() {
  if (key == ' ') { 
    toggleGestureRecording();
  }
}

void toggleGestureRecording() {
  long currentTimestampMs = System.currentTimeMillis();  
  long elapsedTime = currentTimestampMs - _timestampSinceLastGestureRecordToggle;

  if (elapsedTime > MIN_TIME_BETWEEN_GESTURE_TOGGLE_MS) {
    _timestampSinceLastGestureRecordToggle = currentTimestampMs;
    if (_recordingGesture) {
      // if the spacebar was pressed and we're currently recording a gesture
      // then the spacebar just ended the gesture
      _recordingGesture = false;
      _timestampStartCountdownMs = -1;

      GestureRecording curGestureRecording = _gestureRecordings.get(_gestureRecordings.size() - 1);
      curGestureRecording.endTimestamp = currentTimestampMs;
      curGestureRecording.save();

      if (!_mapGestureNameToRecordedCount.containsKey(curGestureRecording.name)) {
        _mapGestureNameToRecordedCount.put(curGestureRecording.name, 1);
      } else {
        int curRecordingCntForGesture = (int)_mapGestureNameToRecordedCount.get(curGestureRecording.name);
        int newRecordingCntForGesture = curRecordingCntForGesture + 1;
        _mapGestureNameToRecordedCount.put(curGestureRecording.name, newRecordingCntForGesture);

        if (newRecordingCntForGesture >= NUM_SAMPLES_TO_RECORD_PER_GESTURE) {
          _curGestureIndex++;
        }
      }
    } else {
      if (_curGestureIndex < GESTURES.length) {
        // if there are still gestures left to record, start countdown timer
        _timestampStartCountdownMs = System.currentTimeMillis();
      }
    }
  }
}

/**
 * Convenience method that returns the number of gestures recorded with the given name
 */
int getNumGesturesRecordedWithName(String name){
  return _mapGestureNameToRecordedCount.containsKey(name) ? (int)_mapGestureNameToRecordedCount.get(name) : 0;
}


void drawDebugInfo() {
  if (_firstSerialValRcvdTimestamp != -1) {
    noStroke();
    fill(255);
    textSize(11);

    float strHeight = textAscent() + textDescent();

    String strSavingTo = "Saving full stream to: " + _fullDataStreamFilenameWithPath;
    float strWidth = textWidth(strSavingTo) + 10;
    float yTextLoc = strHeight;
    text(strSavingTo, width - strWidth, yTextLoc);

    String strDisplayAmt = "Displaying " + nf((DISPLAY_TIMEWINDOW_MS / 1000.0), 2, 1) + " secs";
    strDisplayAmt += " (" + _displaySensorData.size() + " values)";
    strWidth = textWidth(strDisplayAmt) + 10;
    yTextLoc += strHeight;
    text(strDisplayAmt, width - strWidth, yTextLoc);

    long timeElapsedMs = System.currentTimeMillis() - _firstSerialValRcvdTimestamp;
    float samplingRate = _serialLinesRcvd / (timeElapsedMs / 1000.0);


    String strSamplingRate = nf(samplingRate, 3, 1) + " Hz";
    strWidth = textWidth(strSamplingRate) + 10;

    yTextLoc += strHeight;
    text(strSamplingRate, width - strWidth, yTextLoc);

    String strFrameRate = nf(frameRate, 3, 1) + " fps";
    yTextLoc += strHeight;
    text(strFrameRate, width - strWidth, yTextLoc);
  }
}

/**
 * Draws the y-axis ticks and labels
 */ 
void drawYAxis() {
  final int numYTickMarks = 5; 
  final int tickMarkWidth = 6;

  textSize(10);
  float strHeight = textAscent() + textDescent();
  float yRange = getYRange();
  float yTickStep = yRange / numYTickMarks;
  for (int yTickMark = 0; yTickMark < numYTickMarks; yTickMark++) {
    float yVal = map(yTickMark, 0, numYTickMarks, _minSensorVal + yRange * 0.10, _maxSensorVal - yRange * 0.10);
    float yCurPixelVal = getYPixelFromSensorVal(yVal);
    noFill();
    stroke(GRID_COLOR);
    line(0, yCurPixelVal, tickMarkWidth, yCurPixelVal);

    fill(GRID_COLOR);
    noStroke();
    text(nfs(yVal, 3, 1), tickMarkWidth + 2, yCurPixelVal + (strHeight / 2.0) * 0.7);
  }

  color zeroColor = color(255, 255, 255, 50);
  stroke(zeroColor);
  float yVal = 0;
  float yCurPixelVal = getYPixelFromSensorVal(yVal);
  line(0, yCurPixelVal, width, yCurPixelVal);

  noStroke();
  fill(zeroColor);
  String strZero = "0";
  float strWidth = textWidth(strZero);
  text(0, width - strWidth - 2, yCurPixelVal - 2);
}

/**
 * Get full yrange
 */
float getYRange() {
  return _maxSensorVal - _minSensorVal;
}

/**
 * Prints information about the serial port and returns a list of all available serial ports
 */
String[] getAndPrintSerialPortInfo() {
  println("** All Available Serial Ports **");
  String[] listOfSerialPorts = Serial.list();
  printArray(listOfSerialPorts);
  println("** END SERIAL PORT LIST**");

  if(listOfSerialPorts.length > 0){
    String firstPortName = listOfSerialPorts[0];
    println("For example, if your Arduino is on port " + firstPortName + 
      " then you would set ARDUINO_SERIAL_PORT_NAME = " + firstPortName);
  }
  return listOfSerialPorts;
}

/**
 * Convenience method that returns index of serialPortName or -1 if not found 
 */
int getIndexOfArduinoSerialPort(String serialPortName) {
  String[] listOfSerialPorts = Serial.list();
  for (int i=0; i< listOfSerialPorts.length; i++) {
    if (listOfSerialPorts[i].equals(serialPortName)){
      return i;
    }
  }
  return -1;
}

/**
 * Converts a sensor value to a y-pixel value and returns the y-pixel value
 */
float getYPixelFromSensorVal(int sensorVal) {
  return map(sensorVal, _minSensorVal, _maxSensorVal, height, 0);
}

/**
 * Converts a sensor value to a y-pixel value and returns the y-pixel value
 */
float getYPixelFromSensorVal(float sensorVal) {
  return map(sensorVal, _minSensorVal, _maxSensorVal, height, 0);
}

/**
 * Converts a timestamp value to an x-pixel value and returns the x-pixel value
 */
float getXPixelFromTimestamp(long timestamp) {
  return (timestamp - _currentXMin) / (float)DISPLAY_TIMEWINDOW_MS * width;
}

/**
 * Draws a sensor line with the given color
 */
void drawSensorLine(color col, long timestamp1, int sensorVal1, long timestamp2, int sensorVal2) {
  stroke(col);
  strokeWeight(2);
  float xLastPixelVal = getXPixelFromTimestamp(timestamp1);
  float yLastPixelVal = getYPixelFromSensorVal(sensorVal1);
  float xCurPixelVal = getXPixelFromTimestamp(timestamp2);
  float yCurPixelVal = getYPixelFromSensorVal(sensorVal2); 
  line(xLastPixelVal, yLastPixelVal, xCurPixelVal, yCurPixelVal);
}

/**
 * Draws the graph legend, which is dynamic based on the current sensor values
 */
void drawLegend(Rectangle legendRect) {
  if (_displaySensorData.size() <= 0) {
    return;
  }

  color textColor = color(255, 255, 255, 128);

  // draw outline of legend box
  stroke(textColor);
  strokeWeight(1);
  //noFill();
  fill(0, 0, 0, 50);
  rect(legendRect.x, legendRect.y, legendRect.width, legendRect.height);

  // Setup dimension calculations for legend
  int yBuffer = 4;
  int xBuffer = 4;
  int numLegendItems = 3;
  float legendItemHeight = (legendRect.height - (numLegendItems * yBuffer)) / (float)numLegendItems;  
  String [] legendStrs = { "X", "Y", "Z" };
  textSize(legendItemHeight);
  float strHeight = textAscent() + textDescent();
  float yLegendItemPos = legendRect.y + strHeight - textDescent();
  AccelSensorData accelSensorData = _displaySensorData.get(_displaySensorData.size() - 1);
  int [] accelSensorVals = accelSensorData.getSensorValues();

  float titleWidth = textWidth("X");
  float maxValStrWidth = textWidth(Integer.toString(_maxSensorVal));
  float minValStrWidth = textWidth(Integer.toString(_minSensorVal));

  float largestValStrWidth = max(minValStrWidth, maxValStrWidth);

  if (_minSensorVal < 0) {
    // if we have values less than zero, then we split the legend in half
    // and draw the < 0 values to the left of the titles and the > 0
    // to the right of the values

    float xMidLegend = legendRect.x + legendRect.width / 2.0;
    float xTitleStart = xMidLegend - titleWidth / 2.0;
    float maxBarSize = legendRect.width / 2.0 - (2 * xBuffer + titleWidth + 3 * xBuffer + largestValStrWidth);

    for (int i = 0; i < legendStrs.length; i++) {
      String legendStr = legendStrs[i];
      fill(textColor);
      text(legendStr, xTitleStart, yLegendItemPos);

      // draw the bar
      fill(SENSOR_VALUE_COLORS[i]);
      noStroke();
      float barWidth = map(accelSensorVals[i], _minSensorVal, 0, 0, maxBarSize);
      float xBar = xTitleStart - xBuffer - barWidth;
      String strSensorVal = Integer.toString(accelSensorVals[i]);
      float xSensorTextLoc = xBar - largestValStrWidth;
      if (accelSensorVals[i] > 0) {
        barWidth = map(accelSensorVals[i], 0, _maxSensorVal, 0, maxBarSize);
        xBar = xMidLegend + titleWidth / 2.0 + xBuffer;
        xSensorTextLoc = xBar + barWidth + xBuffer;
      }
      rect(xBar, yLegendItemPos - strHeight + textDescent() + yBuffer, barWidth, legendItemHeight - yBuffer); 

      // draw the sensor val
      text(strSensorVal, xSensorTextLoc, yLegendItemPos);
      yLegendItemPos += legendItemHeight + yBuffer;
    }
  } else {
    // no values < 0, so draw legend normally

    float xLegendItemPos = legendRect.x + xBuffer;
    float xBar = xLegendItemPos + titleWidth + xBuffer;
    float maxBarSize = legendRect.width - (xBuffer + titleWidth + 3 * xBuffer + maxValStrWidth);

    // Draw each legend item
    for (int i = 0; i < legendStrs.length; i++) {
      String legendStr = legendStrs[i];
      fill(textColor);
      text(legendStr, xLegendItemPos, yLegendItemPos);

      // draw dynamic legend values
      float barWidth = map(accelSensorVals[i], _minSensorVal, _maxSensorVal, 0, maxBarSize);
      fill(SENSOR_VALUE_COLORS[i]);
      noStroke();
      rect(xBar, yLegendItemPos - strHeight + textDescent() + yBuffer, barWidth, legendItemHeight - yBuffer);
      float xSensorTextLoc = xBar + barWidth + xBuffer;
      text(Integer.toString(accelSensorVals[i]), xSensorTextLoc, yLegendItemPos);
      yLegendItemPos += legendItemHeight + yBuffer;
    }
  }
}

/**
 * Draws an on-screen annotation for recently recorded annotations
 */
void drawGestureRecordingAnnotations() {

  while (_gestureRecordings.size() > 0 && 
    _gestureRecordings.get(0).hasGestureCompleted() &&
    _gestureRecordings.get(0).endTimestamp < _currentXMin) {
    _gestureRecordings.remove(0);
  }

  if (_gestureRecordings.size() <= 0) { 
    return;
  }

  for (GestureRecording gestureRecording : _gestureRecordings) {
    textSize(10);
    fill(255);
    stroke(255);
    strokeWeight(1);

    String strGesture = "Gesture " + (gestureRecording.hasGestureCompleted() ? "Completed:" : "Active:") 
      + "\n" + gestureRecording.name;
    float xPixelStartGesture = getXPixelFromTimestamp(gestureRecording.startTimestamp);
    line(xPixelStartGesture, 0, xPixelStartGesture, height);
    text(strGesture, xPixelStartGesture + 2, 20);  

    if (gestureRecording.hasGestureCompleted()) {
      String strEndGesture = "Gesture End: \n" + gestureRecording.name;
      float xPixelEndGesture = getXPixelFromTimestamp(gestureRecording.endTimestamp);

      noStroke();
      fill(255, 255, 255, 30);
      rect(xPixelStartGesture, 0, xPixelEndGesture - xPixelStartGesture, height);

      stroke(255);
      line(xPixelEndGesture, 0, xPixelEndGesture, height);

      textSize(10);
      fill(255);

      // null check on savedfilename
      // this is because a gesture might be completed but the save to file might not be done
      // see: https://github.com/jonfroehlich/CSE599Sp2019/issues/1
      if (gestureRecording.savedFilename != null) {
        text(gestureRecording.savedFilename, xPixelStartGesture + 2, 50);
      }
    }
  }
}

/**
 * Called automatically when there is data on the serial port
 * See: https://processing.org/reference/libraries/serial/serialEvent_.html
 * Serial source code in github: 
 * https://github.com/processing/processing/blob/master/java/libraries/serial/src/processing/serial/Serial.java
 * 
 * Note: this method is called by a different thread (EventThread) than the draw() method (Animation Thread)
 */
void serialEvent (Serial myPort) {
  //println(Thread.currentThread());

  String inString = "";
  try {
    // Grab the data off the serial port. See: 
    // https://processing.org/reference/libraries/serial/index.html
    inString = trim(_serialPort.readStringUntil('\n'));
    // println(inString);
  }
  catch(Exception e) {
    println("Failed to read serial port. Exception below: ");
    println(e);
    return;
  }

  processInputFromClient(inString);
}

void processServer() {
  if (_wifiServer == null){
    return;
  }
  
  Client thisClient = _wifiServer.available();
  
  // If the client is not null, and says something, display what it said
  if (thisClient !=null) {
    String whatClientSaid = thisClient.readString();
    if (whatClientSaid != null) {
      processInputFromClient(whatClientSaid);
      println(thisClient.ip() + "t" + whatClientSaid);
    }
  } 
  delay(10);
}

// ServerEvent message is generated when a new client connects 
// to an existing server.
void serverEvent(Server someServer, Client someClient) {
  println("We have a new client: " + someClient.ip());
}

/**
 * This processes data from a client (serial or wifi)
 */
void processInputFromClient(String inString) {
  long currentTimestampMs = System.currentTimeMillis();
  _currentXMin = currentTimestampMs - DISPLAY_TIMEWINDOW_MS;

  try {
    if (inString != null) {
      int [] data;

      // Our parser can handle either csv strings or just one float per line
      if (inString.contains(",")) {
        String [] strData = split(inString, ',');
        data = new int[strData.length];
        for (int i=0; i<strData.length; i++) {
          data[i] = int(strData[i].trim());
        }
      } else {
        data = new int[] { int(inString) };
      }

      AccelSensorData accelSensorData = new AccelSensorData(currentTimestampMs, data[0], data[1], data[2], data[3]);
      synchronized(_sensorBuffer) {
        _sensorBuffer.add(accelSensorData);
      }

      // We share the print writer with the UI thread in the exit function
      synchronized (_printWriterAllData) {
        _printWriterAllData.println(accelSensorData.toCsvString());
      }

      _serialLinesRcvd++;

      if (_firstSerialValRcvdTimestamp == -1) {
        _firstSerialValRcvdTimestamp = currentTimestampMs;
      }

      if (data.length >= 4) {
        int gestureRecordButtonPressed = data[4];
        if (gestureRecordButtonPressed == 1) {
          toggleGestureRecording();
        }
      }

      // force the redraw
      //redraw();
    }
  }
  catch(Exception e) {
    println("Received '" + inString + "' but failed to parse. Exception below:");
    println(e);
  }
}

/**
 * Checks for new global min and max data in our sensor values
 */
void checkAndSetNewMinMaxSensorValues(AccelSensorData accelSensorData) {
  int min = min(accelSensorData.x, accelSensorData.y, accelSensorData.z);
  int max = max(accelSensorData.x, accelSensorData.y, accelSensorData.z);
  if (min < _minSensorVal) {
    _minSensorVal = min;
  }

  if (max > _maxSensorVal) {
    _maxSensorVal = max;
  }
}

/**
 * Override exit to flush buffer
 */
void exit() {
  println("Flushing PrintWriter, exiting...");

  if (_serialPort != null) {
    _serialPort.clear();
    _serialPort.stop();
  }

  if (_printWriterAllData != null) {
    // We need to synchronize _printWriterAllData because the serial event
    // where we use _printWriterAllData occurs in a different thread
    synchronized(_printWriterAllData) {
      _printWriterAllData.flush();
      _printWriterAllData.close();
    }
  }

  if (_wifiServer != null) {
    _wifiServer.stop();
  }

  super.exit();
}

// Class for the accelerometer data
class AccelSensorData {
  public final static String CSV_HEADER = "Processing Timestamp (ms), Arduino Timestamp (ms), X, Y, Z";

  public int x;
  public int y;
  public int z;
  public long timestamp;
  public long arduinoTimestamp;

  public AccelSensorData(long timestamp, long arduinoTimestamp, int x, int y, int z) {
    this.timestamp = timestamp;
    this.arduinoTimestamp = arduinoTimestamp;
    this.x = x;
    this.y = y;
    this.z = z;
  }

  // Creates a dynamic array on every call
  public int[] getSensorValues() {
    return new int[] { this.x, this.y, this.z };
  }

  public String toCsvHeaderString() {
    return CSV_HEADER;
  }

  public String toCsvString() {
    return String.format("%d, %d, %d, %d, %d", this.timestamp, this.arduinoTimestamp, this.x, this.y, this.z);
  }

  public String toString() { 
    return String.format("timestamp=%d x=%d y=%d z=%d", this.timestamp, this.x, this.y, this.z);
  }
}

class GestureRecording {
  public long startTimestamp;
  public long endTimestamp = -1;
  public String name;
  public String savedAbsolutePath;
  public String savedFilename;
  public String gestureDir;

  ArrayList<AccelSensorData> listSensorData =  new ArrayList<AccelSensorData>();

  public GestureRecording(String gestureDir, String gestureName, long startTimestamp) {
    this.name = gestureName;
    this.startTimestamp = startTimestamp;
    this.gestureDir = gestureDir;
  }

  public boolean hasGestureCompleted() {
    return this.endTimestamp != -1;
  }

  public void save() {
    long currentTimestampMs = System.currentTimeMillis();
    String filenameNoPath = this.name + "_" + currentTimestampMs + "_" +  this.listSensorData.size() +  ".csv";
    File fDir = new File(sketchPath(this.gestureDir));
    if (!fDir.exists()) {
      fDir.mkdirs();
    }
    File file = new File(fDir, filenameNoPath); 

    println("Attempting to save '" + this.name + "' to " + file.getAbsolutePath());

    try {
      PrintWriter printWriter = new PrintWriter(new BufferedWriter(new FileWriter(file, false)));
      printWriter.println(AccelSensorData.CSV_HEADER);
      for (AccelSensorData accelSensorData : listSensorData) {
        printWriter.println(accelSensorData.toCsvString());
      }
      printWriter.flush();
      printWriter.close();
      println("Wrote " + listSensorData.size() + " lines to " + file.getAbsolutePath());
      this.savedAbsolutePath = file.getAbsolutePath();
      this.savedFilename = file.getName();
    }
    catch (IOException e) {
      e.printStackTrace();
    }
  }
}
