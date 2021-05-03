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

    /**
     * @brief Toggles whether to draw the bounding box around the shape
     * 
     * @param drawBoundingBox If true, then a bounding box will be drawn around the shape
     */
    void setDrawBoundingBox(bool drawBoundingBox){
      _drawBoundingBox = drawBoundingBox;
    }

    /**
     * @brief Toggles whether to draw the object as filled or just an outline
     * 
     * @param drawFill If true, then the shape will be drawn filled (solid)
     */
    void setDrawFill(bool drawFill){
      _drawFill = drawFill;
    }

    /**
     * @brief Convenience function to set location of shape. Simply calls setX(x) and setY(y)
     * 
     * @param x The x location of the shape
     * @param y The y location of the shape
     */
    void setLocation(int x, int y){
      setX(x);
      setY(y);
    }

    /**
     * @brief Sets the y location of the shape
     * 
     * @param y The y location of the shape
     */
    void setY(int y){
      _y = y;
    }

    /**
     * @brief Sets the x location of the shape
     * 
     * @param x The x location of the shape
     */
    void setX(int x){
      _x = x;
    }

    /**
     * @brief Gets the x location of the shape
     * 
     * @return int x location of the shape
     */
    int getX(){
      return _x;
    }

    /**
     * @brief Gets the y location of the shape
     * 
     * @return int y locaiton of the shape
     */
    int getY(){
      return _y;
    }

    /**
     * @brief A virtual function that should be overloaded by sub-classes. Deriving
     * classes should call 
     * 
     * @param display A reference to the Adafruit_SSD1306 object
     */
    virtual void draw(const Adafruit_SSD1306& display) {
      if(_drawBoundingBox){
        display.drawRect(_x, _y, _width, _height, SSD1306_WHITE);
      }
    }

    /**
     * @brief Set the size (width, height) of the shape. This is a virtual
     * function. Deriving classes may want to handle setDimensions differently.
     * For example, the Circle class forces only uses the 'width' parameter
     * and sets height = width.
     * 
     * @param width The width of the shape
     * @param height The height of the shape
     */
    virtual void setDimensions(int width, int height){
      _width = width;
      _height = height;
    }

    /**
     * @brief Get the width of the shape
     * 
     * @return int Width of the shape in pixels
     */
    int getWidth(){
      return _width;
    }

    /**
     * @brief Get the height of the shape
     * 
     * @return int Height of the shape in pixels
     */
    int getHeight(){
      return _height;
    }

    /**
     * @brief Get the left (x) location of the shape, which is just getX()
     * 
     * @return int The left location of the shape
     */
    int getLeft() {
      return _x;
    }

    /**
     * @brief Get the right (x) location, which is getX() + getWidth()
     * 
     * @return int The right location (x) of the shape
     */
    int getRight() {
      return _x + _width;
    }

    /**
     * @brief Get the bottom of the shape (y), which is getY() + getHeight()
     * 
     * @return int 
     */
    int getBottom() {
      return _y + _height;
    }

    /**
     * @brief Get the top of the shape, which is just getY()
     * 
     * @return int 
     */
    int getTop() {
      return _y;
    }

    /**
     * @brief Attempts to force the x,y location of the shape into the
     * provided rectangular boundary (x, y, width, height)
     * 
     * @param x X location of boundary
     * @param y Y location of boundary
     * @param width Width of boundary
     * @param height Height of boundary
     */
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

    /**
     * @brief Checks to see if the current shape overlaps with the provided shape.
     * Overlap is defined as any pixel between this shape touching (equaling)
     * the passed in shape
     * 
     * @param shape The shape to check for overlap
     * @return true If the shape overlaps
     * @return false If the shape does not overlap
     */
    virtual bool overlaps(const Shape& shape) {
      //Serial.println("We are in overlaps shape!");

      // based on https://stackoverflow.com/a/4098512
      return !(getRight() < shape._x ||
               getBottom() < shape._y ||
               _x > shape.getRight() ||
               _y > shape.getBottom());
    }

    /**
     * @brief Checks if this shape contains the x,y location. If so, returns true.
     * 
     * @param x The X location to check
     * @param y The Y location to check
     * @return true If this shape contains the x,y
     * @return false If this shape does NOT contain the x,y
     */
    virtual bool contains(int x, int y) {
      return x >= _x && // check within left edge
             x <= (_x + _width) && // check within right edge
             y >= _y && // check within top edge
             y <= (_y + _height); // check within bottom edge
    }

    /**
     * @brief Gets the name of this object. Sub-classes should override this.
     * 
     * @return String 
     */
    virtual String getName(){
      return "Shape";
    }

    /**
     * @brief Gets a basic toString() for the object. Sub-classes should override
     * to get a more fitting description
     * 
     * @return String 
     */
    virtual String toString(){
      return (String)"x: " + _x + " y: " + _y + " width: " + _width + " height: " + _height;
    }

    /**
     * @brief Static function that calculates the Euclidean distance between two points
     * 
     * @param x1 
     * @param y1 
     * @param x2 
     * @param y2 
     * @return float 
     */
    static float distance(int x1, int y1, int x2, int y2){
      return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    }
};

/**
 * @brief A Rectangle shape class that extends Shape.
 * 
 */
class Rectangle : public Shape {
  public:
    Rectangle(int x, int y, int width, int height) : Shape(x, y, width, height)
    {
    }

    /**
     * @brief Draw the rectangle
     * 
     * @param display 
     */
    void draw (const Adafruit_SSD1306& display) override{
      // Draw rectangle takes in (xTop, yTop, width, height)
      // https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#rectangles-2002784-10
      if(_drawFill){
        display.fillRect(_x, _y, _width, _height, SSD1306_WHITE);
      }else{
        display.drawRect(_x, _y, _width, _height, SSD1306_WHITE);
      }

      // We don't call the parent class draw() call because
      // the bounding box is simple another rectangle
      // Shape::draw(disp);
    }

    String getName() override{
      return "Rectangle";
    }
};

/**
 * @brief A circular shape class that extends Shape
 * 
 */
class Circle : public Shape {

  public:
    Circle(int xCenter, int yCenter, int radius) : Shape(xCenter - radius, yCenter - radius, (radius * 2) + 1, (radius * 2) + 1)    
    {

    }

    /**
     * @brief Draw the circle
     * 
     * @param display 
     */
    void draw(const Adafruit_SSD1306& disp) override{
      // Draw circle takes in (xCenter, yCenter, radius)
      // https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#circles-2002788-14
      int radius = getRadius();
      if(_drawFill){
        disp.fillCircle(_x + radius, _y + radius, radius, SSD1306_WHITE);
      }else{
        disp.drawCircle(_x + radius, _y + radius, radius, SSD1306_WHITE);
      }

      // Call super method
      Shape::draw(disp);
    }

    /**
     * @brief Calculates the overlap between two circular objects
     * 
     * @param circle 
     * @return true 
     * @return false 
     */
    bool overlaps(const Circle& circle) {
      int distanceFromCenterPoints = Shape::distance(getCenterX(), getCenterY(), circle.getCenterX(), circle.getCenterY());
      return distanceFromCenterPoints <= getRadius() + circle.getRadius();
    }

    /**
     * @brief Calculates the overlap between two shapes
     * 
     * @param shape 
     * @return true 
     * @return false 
     */
    bool overlaps(const Shape& shape) override{
      if(getName().equals(shape.getName())){
        return this->overlaps((Circle&)shape);
      }
      
      // Default to parent overlaps function
      return Shape::overlaps(shape);
    }

    /**
     * @brief Uses the width value to set the circle diameter. The height parameter is ignored.
     * 
     * @param width sets the circle diameter
     * @param height ignored
     */
    void setDimensions(int width, int height) override{
      Shape::setDimensions(width, width);
    }

    /**
     * @brief Get the X center of the circle
     * 
     * @return int 
     */
    int getCenterX(){
      return _x + getWidth() / 2;
    }

    /**
     * @brief Get the Y center of the circle
     * 
     * @return int 
     */
    int getCenterY(){
      return _y + getWidth() / 2;
    }

    /**
     * @brief Moves the location of the object so that the center is at x,y
     * 
     * @param x 
     * @param y 
     * @return int 
     */
    int setCenter(int x, int y){
      int radius = getRadius();
      setLocation(x - radius, y - radius);
    }

    /**
     * @brief Get the radius of the circle
     * 
     * @return int 
     */
    int getRadius(){
      return getWidth() / 2;
    }

    /**
     * @brief Set the radius of the circle
     * 
     * @param radius 
     */
    void setRadius(int radius){
      int size = 2 * radius;
      setDimensions(size, size);
    }
};

class Ball : public Circle{

  protected:
    int _xSpeed = 0;
    int _ySpeed = 0;
  
  public:
    Ball(int xCenter, int yCenter, int radius) : Circle(xCenter, yCenter, radius)    
    {

    }

    boolean checkYBounce(int yMin, int yMax){
      return getTop() <= yMin || getBottom() >= yMax;
    }

    boolean checkXBounce(int xMin, int xMax){
      return getLeft() <= xMin || getRight() >= xMax;
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

    String getName() override{
      return "Ball";
    }
};
