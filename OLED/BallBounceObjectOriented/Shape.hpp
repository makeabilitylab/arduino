#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Shape {

  protected:
    int _x;
    int _y;
    int _width;
    int _height;

  public:
    Shape(int x, int y, int width, int height) {
      _x = x;
      _y = y;
      _width = width;
      _height = height;
    }

//    void update(){
//
//    }

    void draw(Adafruit_SSD1306 display) {
      // empty
    }

    int getWidth(){
      return _width;
    }

    int getHeight(){
      return _height;
    }

    int getLeft() {
      return _x;
    }

    int getRight() {
      return _x + _width;
    }

    int getBottom() {
      return _y + _height;
    }

    int getTop() {
      return _y;
    }

    boolean overlaps(Shape shape) {
      // based on https://stackoverflow.com/a/4098512
      return !(this->getRight() < shape._x ||
               this->getBottom() < shape._y ||
               this->_x > shape.getRight() ||
               this->_y > shape.getBottom());
    }

    boolean contains(int x, int y) {
      return x >= _x && // check within left edge
             x <= (_x + _width) && // check within right edge
             y >= _y && // check within top edge
             y <= (_y + _height); // check within bottom edge
    }
};

// Derived class
class Ball : public Shape {

  protected:
    int _radius;
    int _xSpeed;
    int _ySpeed;
  
  public:
    Ball(int x, int y, int radius) : Shape(x - radius, y - radius, radius * 2, radius * 2)    
    {
        _radius = radius;

        // Gets a random long between min and max - 1
        // https://www.arduino.cc/reference/en/language/functions/random-numbers/random/
        _xSpeed = random(1, 4);
        _ySpeed = random(1, 4);
    }
  
    void draw(const Adafruit_SSD1306& disp) {
      // Draw circle takes in (xCenter, yCenter, radius)
      // https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#circles-2002788-14
      disp.drawCircle(_x + _radius, _y + _radius, _radius, SSD1306_WHITE);
    }

    void update(){
      _x += _xSpeed;
      _y += _ySpeed;
    }

    int reverseY(){
      _ySpeed = _ySpeed * -1;
      return _ySpeed;
    }

    int reverseX(){
      _xSpeed = _xSpeed * -1;
      return _xSpeed;
    }

    boolean checkYBounce(int yMin, int yMax){
      return getTop() <= yMin || getBottom() >= yMax;
    }

    boolean checkXBounce(int xMin, int xMax){
      return getLeft() <= xMin || getRight() >= xMax;
    }
};
