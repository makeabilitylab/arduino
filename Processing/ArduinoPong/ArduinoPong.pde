/* A basic pong game intended to be used with ArduinoPong.ino
 *
 * FUTURE IDEAS:
 *   - Have a ball trail like /Demos/Graphics/Reflection2 or perhaps /Topics/Simualte/SmokeParticleSystem
 *   - Have particle explosion when ball hits paddle (see /Demos/Graphics/Particles)
 *      -- See also /Topics/Simulate/MultipleParticleSystems
 *   - Randomly cut paddle in half to make it harder? Or maybe do this when other team is ahead?
 *   - Release multiple balls into game play (maybe ball splits sometimes when it hits paddle?)
 *   - Add in directional lighting (see /Basics/Directional)
 *   - Could use an animated sprite as a ball (see /Topics/Animation/AnimatedSprite)
 *   - Perhaps a bouncing ball/rectangle (or more than one) is injected into gameplay and pong ball bounces off of it
 *   - record sound of four or five different ball knocking sounds and use them randomly when paddle hits ball
 *
 * By Jon Froehlich
 * http://makeabilitylab.io
 *
 */

import processing.serial.*;

// We communicate with the Arduino via the serial port
Serial _serialPort;

final int ARDUINO_SERIAL_PORT_INDEX = 7;
final int DEFAULT_PADDLE_WIDTH = 25;
final int DEFAULT_PADDLE_HEIGHT = 120;
final int DEFAULT_BALL_SIZE = 20;

Ball _ball;
Paddle _leftPaddle;
Paddle _rightPaddle;
int _scoreLeft = 0;
int _scoreRight = 0;

// Used for loading a font, see https://processing.org/tutorials/text/
PFont _font;

void setup(){
  size(640, 480);
  
  _font = createFont("Arial", 64, false); 
  textFont(_font);
  
  background(102);
  
  _ball = new Ball(50, 50, DEFAULT_BALL_SIZE, DEFAULT_BALL_SIZE);
  _leftPaddle = new Paddle(0, height / 2, DEFAULT_PADDLE_WIDTH, DEFAULT_PADDLE_HEIGHT);
  _rightPaddle = new Paddle(width - DEFAULT_PADDLE_WIDTH, height / 2, DEFAULT_PADDLE_WIDTH, DEFAULT_PADDLE_HEIGHT);
  
  // Open the serial port
  _serialPort = new Serial(this, Serial.list()[ARDUINO_SERIAL_PORT_INDEX], 9600);

  // Don't generate a serialEvent() unless you get a newline character:
  _serialPort.bufferUntil('\n');
}

void draw(){
  background(51);
  
  drawScoreboard();
  
  _ball.update(width, height, _leftPaddle, _rightPaddle);
  
  if(_ball.x < 0 || _ball.x + _ball.width > width){
    if(_ball.x < 0){
      println("Right scored a point!");
      _scoreRight++;
    }else if(_ball.x + _ball.width > width){
      println("Left scored a point!");
      _scoreLeft++;
    }
    println("Left score: " + _scoreLeft + " Right score: " + _scoreRight);
    _ball.reset(width, height);
  }
  
  _ball.draw();
  _leftPaddle.draw();
  _rightPaddle.draw();
}

void drawScoreboard(){
  //draw the score
  fill(255, 255, 255, 200);
  float strHeight = textAscent() + textDescent();
  
  String strLeftScore = _scoreLeft + "";
  float strWidth = textWidth(strLeftScore);
  float xPos = width / 2.0 - strWidth - 20;
  text(strLeftScore, xPos, 10 + strHeight - textDescent() - 2);
  
  String strRightScore = _scoreRight + "";
  strWidth = textWidth(strRightScore);
  xPos = width / 2.0 + strWidth + 20;
  text(strRightScore, xPos, 10 + strHeight - textDescent() - 2);
}

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
      
      float leftPaddleYFrac = data[0];
      float rightPaddYFrac = data[1];
      
      _leftPaddle.y = (int)map(leftPaddleYFrac, 0, 1, 0, height - _leftPaddle.height);
      _rightPaddle.y = (int)map(rightPaddYFrac, 0, 1, 0, height - _rightPaddle.height);
    }

    // Print out the received data (this is just for debugging)
    // printArray(data);
  }
  catch(Exception e) {
    println(e);
  }
}
