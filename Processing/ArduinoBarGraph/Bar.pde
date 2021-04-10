/* 
 * A simple bar graph class
 * 
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 */

class Bar{
  float x;
  float barHeight;
  float barWidth;
  String label;
  
  Bar(float x, float barWidth, float barHeight, String label){
    this.x = x;
    this.barHeight = barHeight;
    this.barWidth = barWidth;
    this.label = label;
  }
  
  public void draw(){
    // change color dynamically depending on bar height
    int redColor = (int)map(this.barHeight, 0, height, 0, 255);
    fill(redColor, 34, 255); 
    
    // draw the bar (as a rectangle).
    rect(this.x, height - this.barHeight, this.barWidth, this.barHeight);
    
    // draw the bar label
    float strWidth = textWidth(this.label);
    fill(255);
    text(this.label, this.x + this.barWidth / 2.0 - strWidth / 2.0, height - textDescent());
  }
}
