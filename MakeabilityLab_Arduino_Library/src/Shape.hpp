#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Shape {

  protected:
    int _x;
    int _y;
    int _width;
    int _height;
    bool _drawFill = false;
    bool _drawBoundingBox = false;

  public:
    Shape(int x, int y, int width, int height) {
      _x = x;
      _y = y;
      _width = width;
      _height = height;
    }

    void setDrawBoundingBox(bool drawBoundingBox){
      _drawBoundingBox = drawBoundingBox;
    }

    void setDrawFill(bool drawFill){
      _drawFill = drawFill;
    }

    void setLocation(int x, int y){
      setX(x);
      setY(y);
    }

    void setY(int y){
      _y = y;
    }

    void setX(int x){
      _x = x;
    }

    int getX(){
      return _x;
    }

    int getY(){
      return _y;
    }

    virtual void draw(const Adafruit_SSD1306& display) {
      if(_drawBoundingBox){
        display.drawRect(_x, _y, _width, _height, SSD1306_WHITE);
      }
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

    void forceInside(int x, int y, int width, int height){
      if(getTop() <= y){
        setY(y);
      }else if(getBottom() >= y + height){
        setY((y + height) - getHeight() - 1);
      }

      if(getLeft() <= x){
        setX(x);
      }else if(getRight() >= x + width){
        setX((x + width) - getWidth() - 1);
      }
    }

    virtual bool overlaps(const Shape& shape) {
      Serial.println("We are in overlaps shape!");

      // based on https://stackoverflow.com/a/4098512
      return !(getRight() < shape._x ||
               getBottom() < shape._y ||
               _x > shape.getRight() ||
               _y > shape.getBottom());
    }

    virtual bool contains(int x, int y) {
      return x >= _x && // check within left edge
             x <= (_x + _width) && // check within right edge
             y >= _y && // check within top edge
             y <= (_y + _height); // check within bottom edge
    }

    virtual String getName(){
      return "Shape";
    }

    virtual String toString(){
      return (String)"x: " + _x + " y: " + _y + " width: " + _width + " height: " + _height;
    }

    static float distance(int x1, int y1, int x2, int y2){
      return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    }
};

  
class Rectangle : public Shape {
  public:
    Rectangle(int x, int y, int width, int height) : Shape(x, y, width, height)
    {
    }

    void draw (const Adafruit_SSD1306& display) override{
      // Draw rectangle takes in (xTop, yTop, width, height)
      // https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#rectangles-2002784-10
      if(_drawFill){
        display.fillRect(_x, _y, _width, _height, SSD1306_WHITE);
      }else{
        display.drawRect(_x, _y, _width, _height, SSD1306_WHITE);
      }
    }

    String getName() override{
      return "Rectangle";
    }
};

// Derived class
class Ball : public Shape {

  protected:
    int _radius;
    int _xSpeed = 0;
    int _ySpeed = 0;
  
  public:
    Ball(int xCenter, int yCenter, int radius) : Shape(xCenter - radius, yCenter - radius, (radius * 2) + 1, (radius * 2) + 1)    
    {
        _radius = radius;

        // Gets a random long between min and max - 1
        // https://www.arduino.cc/reference/en/language/functions/random-numbers/random/
        //_xSpeed = random(1, 4);
        //_ySpeed = random(1, 4);
    }

    /**
     * @brief 
     * 
     * @param display 
     */
    void draw(const Adafruit_SSD1306& disp) override{
      // Draw circle takes in (xCenter, yCenter, radius)
      // https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#circles-2002788-14
      if(_drawFill){
        disp.fillCircle(_x + _radius, _y + _radius, _radius, SSD1306_WHITE);
      }else{
        disp.drawCircle(_x + _radius, _y + _radius, _radius, SSD1306_WHITE);
      }

      // Call super method
      Shape::draw(disp);
    }

    bool overlaps(const Ball& ball) {
      int distanceFromCenterPoints = Shape::distance(getCenterX(), getCenterY(), ball.getCenterX(), ball.getCenterY());
      return distanceFromCenterPoints <= getRadius() + ball.getRadius();
    }

    bool overlaps(const Shape& shape) override{
      if(getName().equals(shape.getName())){
        const Ball* ball = (Ball*)&shape;
        return this->overlaps((Ball&)shape);
      }
      
      // Default to parent overlaps function
      return Shape::overlaps(shape);
    }

    int getCenterX(){
      return _x + _radius;
    }

    int getCenterY(){
      return _y + _radius;
    }

    int setCenter(int x, int y){
      setLocation(x - _radius, y - _radius);
    }

    int getRadius(){
      return _radius;
    }

    void setRadius(int radius){
      _radius = radius;
      _width = 2 * _radius;
      _height = 2 * _radius;
    }

    void setSpeed(int xSpeed, int ySpeed){
      _xSpeed = xSpeed;
      _ySpeed = ySpeed;
    }

    int getXSpeed(){
      return _xSpeed;
    }

    int getYSpeed(){
      return _ySpeed;
    }

    void update(){
      _x += _xSpeed;
      _y += _ySpeed;
    }

    int reverseYSpeed(){
      _ySpeed = _ySpeed * -1;
      return _ySpeed;
    }

    int reverseXSpeed(){
      _xSpeed = _xSpeed * -1;
      return _xSpeed;
    }

    boolean checkYBounce(int yMin, int yMax){
      return getTop() <= yMin || getBottom() >= yMax;
    }

    boolean checkXBounce(int xMin, int xMax){
      return getLeft() <= xMin || getRight() >= xMax;
    }

    String getName() override{
      return "Ball";
    }
};
