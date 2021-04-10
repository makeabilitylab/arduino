
class Paddle extends java.awt.geom.Rectangle2D.Float{
  Paddle(int x, int y, int rectWidth, int rectHeight){
    super(x, y, rectWidth, rectHeight);
  }
  
  public void draw(){
    fill(255, 0, 255);
    rect(this.x, this.y, this.width, this.height);
  }
}
