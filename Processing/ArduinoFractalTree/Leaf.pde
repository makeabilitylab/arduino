/* 
 * The leaf object
 *
 * By Jon Froehlich
 * http://makeabilitylab.io
 */
class Leaf{
  public PVector loc;
  float leafWidth = random(2,12);
  float leafHeight = random(2,12);
  color fillColor = color(255, 255, 255, random(100, 255));
  
  Leaf(PVector pt){
    this.loc = pt;
  }
  
  public void draw(){
    fill(this.fillColor);
    ellipse(this.loc.x, this.loc.y, this.leafWidth, this.leafHeight);
  }
}
