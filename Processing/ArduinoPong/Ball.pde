
class Ball extends java.awt.geom.Rectangle2D.Float{
  float _xSpeed = random(2, 5); // speed is in pixels/frame
  float _ySpeed = random(2, 5); // speed is in pixels/frame
  
  Ball(int x, int y, int rectWidth, int rectHeight){
    super(x, y, rectWidth, rectHeight);
  }
  
  public void update(int screenWidth, int screenHeight, Paddle leftPaddle, Paddle rightPaddle){
    this.x = this.x + _xSpeed;
    this.y = this.y + _ySpeed;
    
    if(this.y < 0 || (this.y + this.height) > screenHeight){
      _ySpeed *= -1;
    }
    
    if(leftPaddle.contains(this.x - 1, this.y)){
      _xSpeed *= -1;
      this.x = leftPaddle.x + leftPaddle.width + 1;
    }
    
    if(rightPaddle.contains(this.x + this.width, this.y)){
      _xSpeed *= -1;
      this.x = rightPaddle.x - 1 - this.width;
    }
  }
  
  public void reset(int screenWidth, int screenHeight){
    this.x = screenWidth / 2.0;
    this.y = screenHeight / 2.0;
    _xSpeed = random(2, 5); // speed is in pixels/frame
    _ySpeed = random(2, 5); // speed is in pixels/frame 
  }
  
  public void draw(){
    fill(255, 0, 255);
    rect(this.x, this.y, this.width, this.height);
  }
}
