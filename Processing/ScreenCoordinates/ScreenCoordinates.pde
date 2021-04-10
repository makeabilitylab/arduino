/*
 * This example shows how the screen coordinates are oriented in Processing. The
 * top-left corner of the window is 0,0 and bottom-right is (width, height) so
 * higher x values move right and higher y values move down.
 *   
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 */

// Used for loading a font, see https://processing.org/tutorials/text/
PFont _font; 

void setup(){
  size(640,480);
  _font = createFont("Arial", 16, true); // Arial, 16 point, anti-aliasing on
}

void draw(){
  background(10);
  
  // draw semi-translucent gray lines through center of screen
  stroke(128, 128, 128, 128); // set color to gray
  line(0, height / 2.0, width, height / 2.0); 
  line(width / 2.0, 0, width / 2.0, height);
  
  textFont(_font); // all text will be drawn with this font, so can set it here
  fill(255);
  float strHeight = textAscent() + textDescent();
  
  // draw top left corner
  String strPixelCoordinates = String.format("(x=%d,y=%d)", 0, 0);
  float strWidth = textWidth(strPixelCoordinates);
  text(strPixelCoordinates, 0, strHeight - textDescent() - 2);
  
  // draw top right corner
  strPixelCoordinates = String.format("(x=%d,y=0)", width - 1);
  strWidth = textWidth(strPixelCoordinates);
  text(strPixelCoordinates, width - strWidth, strHeight - textDescent() - 2);
  
  // draw bottom right corner
  strPixelCoordinates = String.format("(x=%d,y=%d)", width - 1, height - 1);
  strWidth = textWidth(strPixelCoordinates);
  text(strPixelCoordinates, width - strWidth, height - textDescent());
  
  // draw bottom left corner
  strPixelCoordinates = String.format("(x=%d,y=%d)", 0, height - 1);
  strWidth = textWidth(strPixelCoordinates);
  text(strPixelCoordinates, 0, height - textDescent());
  
  // draw center of screen
  strPixelCoordinates = String.format("(x=%.1f,y=%.1f)", width / 2.0, height / 2.0);
  strWidth = textWidth(strPixelCoordinates);
  
  fill(128, 128, 128, 128);
  rect(width / 2.0 - strWidth / 2.0, height / 2.0 - strHeight / 2.0, strWidth, strHeight);
  
  fill(255, 255, 0);
  text(strPixelCoordinates, width / 2.0 - strWidth / 2.0, height / 2.0 + strHeight / 2.0 - textDescent());
  
  // draw position of mouse
  strPixelCoordinates = String.format("(x=%d,y=%d)", mouseX, mouseY);
  strWidth = textWidth(strPixelCoordinates);
  float xPos = mouseX;
  if(mouseX + strWidth > width){
    xPos = width - strWidth;
  }
  float yPos = mouseY;
  if(mouseY - strHeight < 0){
     yPos = strHeight - textDescent();
  }
  text(strPixelCoordinates, xPos, yPos);
}
