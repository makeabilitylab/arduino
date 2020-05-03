/*
 * This example prints the serial ports to the screen
 *   
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

import processing.serial.*;

// Serial is necessary to communicate with the Arduino over the serial port
Serial _serialPort;

void setup(){
  size(400, 200);
  
  // Print to console all the available serial ports
  printArray(Serial.list());
  
  noLoop(); // no need to loop
}

void draw(){
  background(10);
  
  
  float strHeight = textAscent() + textDescent();
  String [] serialPorts = Serial.list();
  float y = strHeight;
  int mostLikelyIndex = -1;
  for(int i = 0; i < serialPorts.length; i++){
    fill(255);
 
    String strPortIndexAndName = String.format("[%d] : %s", i, serialPorts[i]); 
    if(strPortIndexAndName.toLowerCase().contains("cu.usbmodem14601")){
      fill(0, 220, 0);
      strPortIndexAndName += " <- most likely index is " + i;
      mostLikelyIndex = i;
    }
    
    text(strPortIndexAndName, 5, y);
    y += strHeight;
  }
  
  if(mostLikelyIndex != -1){
    print(String.format("\nThe most likely port index is '%d' with name %s",
                        mostLikelyIndex, serialPorts[mostLikelyIndex]));
  }
 
}
