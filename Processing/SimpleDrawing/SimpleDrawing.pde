void setup(){
  // Set the canvas size
  size(1100, 900);
}

void draw(){
  // Change "paintbrush" fill color to either white
  // or black depending on whether mouse is pressed
  if(mousePressed){
    fill(0); // fill color black
  }else{
    fill(255); // fill color white
  }
  circle(mouseX, mouseY, 100);
}
