/**
 * @file Shape.hpp
 * @brief A basic shape library for monochrome OLEDs using Adafruit_SSD1306.h, which is 
 * Adafruit's library based on the SSD1306 drivers.
 *
 * Provides a Shape base class with Rectangle, RoundRect, Circle, Triangle, and Ball
 * subclasses for drawing on monochrome OLED displays. Includes bounding-box collision
 * detection, containment testing, boundary clamping, and per-shape velocity for
 * building simple games and animations.
 *
 * Video demo:
 * https://youtu.be/-dLtvL1bOn8
 *
 * See also:
 * LineGraph.hpp
 * ScrollingLineGraph.hpp
 *
 * @section usage Usage Example
 * @code
 *   Circle ball(64, 32, 5);
 *   Rectangle paddle(0, 28, 5, 8);
 *
 *   ball.setDrawFill(true);
 *   ball.setSpeed(1, 1);
 *
 *   void loop() {
 *     ball.update();
 *
 *     if (ball.isOutOfBoundsX(0, 128)) ball.reverseXSpeed();
 *     if (ball.isOutOfBoundsY(0, 64))  ball.reverseYSpeed();
 *     ball.forceInside(0, 0, 128, 64);
 *
 *     if (ball.overlaps(paddle)) {
 *       ball.reverseXSpeed();
 *     }
 *
 *     display.clearDisplay();
 *     ball.draw(display);
 *     paddle.draw(display);
 *     display.display();
 *   }
 * @endcode
 *
 * @author Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/advancedio/oled.html
 */
#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/**
 * @brief Identifies the type of shape for fast, allocation-free type checking.
 *
 * Used internally by overlaps() to select the most precise collision algorithm
 * (e.g., radius-based overlap for two circles). Using an enum avoids the heap
 * fragmentation that Arduino String comparisons would cause on memory-constrained
 * boards like the Uno (2KB SRAM).
 */
enum ShapeType {
  SHAPE,
  RECTANGLE,
  ROUND_RECT,
  TRIANGLE,
  CIRCLE
};

class Shape {

  protected:
    int _x;
    int _y;
    int _width;
    int _height;
    bool _drawFill = false;
    bool _drawBoundingBox = false;

    // Velocity support for all shapes. Every shape carries these 4 bytes
    // even if it never moves. For typical student games with 5-10 objects
    // this is fine; be mindful on very memory-constrained boards (e.g.,
    // Arduino Uno with 2KB SRAM) if creating large arrays of shapes.
    int _xSpeed = 0;  ///< Horizontal speed in pixels per update() call
    int _ySpeed = 0;  ///< Vertical speed in pixels per update() call

  public:

    Shape(int x, int y, int width, int height)
      : Shape(x, y, width, height, false) {
      // purposefully empty
    }

    Shape(int x, int y, int width, int height, bool drawFillOn) {
      _x = x;
      _y = y;
      _width = width;
      _height = height;
      _drawFill = drawFillOn;
    }

    /**
     * @brief Virtual destructor. Required because Shape is a polymorphic base
     * class with virtual functions. Without this, deleting a derived class
     * through a Shape pointer (e.g., `delete myShapePtr;`) would cause
     * undefined behavior.
     */
    virtual ~Shape() = default;

    /**
     * @brief Toggles whether to draw the bounding box around the shape
     *
     * @param drawBoundingBox If true, then a bounding box will be drawn around the shape
     */
    void setDrawBoundingBox(bool drawBoundingBox) {
      _drawBoundingBox = drawBoundingBox;
    }

    /**
     * @brief Toggles whether to draw the object as filled or just an outline
     *
     * @param drawFill If true, then the shape will be drawn filled (solid)
     */
    void setDrawFill(bool drawFill) {
      _drawFill = drawFill;
    }

    /**
     * @brief Convenience function to set location of shape. Simply calls setX(x) and setY(y)
     *
     * @param x The x location of the shape
     * @param y The y location of the shape
     */
    void setLocation(int x, int y) {
      setX(x);
      setY(y);
    }

    /**
     * @brief Sets the y location of the shape. Virtual so subclasses like
     * Triangle can override to move internal geometry (e.g., vertices).
     * @param y The y location of the shape
     */
    virtual void setY(int y) {
      _y = y;
    }

    /**
     * @brief Sets the x location of the shape. Virtual so subclasses like
     * Triangle can override to move internal geometry (e.g., vertices).
     * @param x The x location of the shape
     */
    virtual void setX(int x) {
      _x = x;
    }

    /**
     * @brief Gets the x location of the shape
     * @return int x location of the shape
     */
    int getX() const {
      return _x;
    }

    /**
     * @brief Gets the y location of the shape
     * @return int y location of the shape
     */
    int getY() const {
      return _y;
    }

    /**
     * @brief A virtual function that should be overloaded by sub-classes. Deriving
     * classes should call Shape::draw(display) if they want the bounding box drawn.
     *
     * @param display A reference to the Adafruit_SSD1306 object
     */
    virtual void draw(Adafruit_SSD1306& display) const {
      if (_drawBoundingBox) {
        display.drawRect(_x, _y, _width, _height, SSD1306_WHITE);
      }
    }

    /**
     * @brief Set the size (width, height) of the shape. This is a virtual
     * function. Deriving classes may want to handle setDimensions differently.
     * For example, the Circle class only uses the 'width' parameter
     * and sets height = width.
     *
     * @param width The width of the shape
     * @param height The height of the shape
     */
    virtual void setDimensions(int width, int height) {
      _width = width;
      _height = height;
    }

    /**
     * @brief Get the width of the shape
     * @return int Width of the shape in pixels
     */
    int getWidth() const {
      return _width;
    }

    /**
     * @brief Get the height of the shape
     * @return int Height of the shape in pixels
     */
    int getHeight() const {
      return _height;
    }

    /**
     * @brief Get the left (x) location of the shape, which is just getX()
     * @return int The left location of the shape
     */
    int getLeft() const {
      return _x;
    }

    /**
     * @brief Get the right (x) location, which is getX() + getWidth()
     * @return int The right location (x) of the shape
     */
    int getRight() const {
      return _x + _width;
    }

    /**
     * @brief Get the bottom of the shape (y), which is getY() + getHeight()
     * @return int
     */
    int getBottom() const {
      return _y + _height;
    }

    /**
     * @brief Get the top of the shape, which is just getY()
     * @return int
     */
    int getTop() const {
      return _y;
    }

    /**
     * @brief Get the X coordinate of the shape's center.
     * @return int The horizontal center of the bounding box
     */
    int getCenterX() const {
      return _x + _width / 2;
    }

    /**
     * @brief Get the Y coordinate of the shape's center.
     * @return int The vertical center of the bounding box
     */
    int getCenterY() const {
      return _y + _height / 2;
    }

    // ── Velocity ────────────────────────────────────────────────────────

    /**
     * @brief Set the shape's speed in pixels per update() call.
     *
     * @param xSpeed Horizontal speed (positive = right, negative = left)
     * @param ySpeed Vertical speed (positive = down, negative = up)
     */
    void setSpeed(int xSpeed, int ySpeed) {
      _xSpeed = xSpeed;
      _ySpeed = ySpeed;
    }

    int getXSpeed() const {
      return _xSpeed;
    }

    int getYSpeed() const {
      return _ySpeed;
    }

    /**
     * @brief Advance the shape's position by its current speed.
     *        Call once per frame in your game loop.
     *
     * Uses setX()/setY() rather than modifying _x/_y directly so that
     * subclasses that override setX()/setY() (like Triangle, which needs
     * to move its vertices) work correctly.
     */
    void update() {
      setX(_x + _xSpeed);
      setY(_y + _ySpeed);
    }

    /**
     * @brief Negate the horizontal speed (e.g., after hitting a wall).
     * @return int The new X speed
     */
    int reverseXSpeed() {
      _xSpeed = _xSpeed * -1;
      return _xSpeed;
    }

    /**
     * @brief Negate the vertical speed (e.g., after hitting a floor/ceiling).
     * @return int The new Y speed
     */
    int reverseYSpeed() {
      _ySpeed = _ySpeed * -1;
      return _ySpeed;
    }

    // ── Bounds checking ─────────────────────────────────────────────────

    /**
     * @brief Check if the shape has reached or exceeded a horizontal boundary.
     * Useful for detecting when any moving shape (ball, paddle, enemy, etc.)
     * has hit the left or right edge of the screen or play area.
     *
     * @param xMin Left boundary (inclusive)
     * @param xMax Right boundary (inclusive)
     * @return true if the shape's left edge <= xMin or right edge >= xMax
     */
    bool isOutOfBoundsX(int xMin, int xMax) const {
      return getLeft() <= xMin || getRight() >= xMax;
    }

    /**
     * @brief Check if the shape has reached or exceeded a vertical boundary.
     * Useful for detecting when any moving shape has hit the top or bottom
     * edge of the screen or play area.
     *
     * @param yMin Top boundary (inclusive)
     * @param yMax Bottom boundary (inclusive)
     * @return true if the shape's top edge <= yMin or bottom edge >= yMax
     */
    bool isOutOfBoundsY(int yMin, int yMax) const {
      return getTop() <= yMin || getBottom() >= yMax;
    }

    /**
     * @brief Check if the shape has reached or exceeded any boundary.
     * Convenience method that checks both axes at once.
     *
     * @param xMin Left boundary (inclusive)
     * @param yMin Top boundary (inclusive)
     * @param xMax Right boundary (inclusive)
     * @param yMax Bottom boundary (inclusive)
     * @return true if out of bounds on either axis
     */
    bool isOutOfBounds(int xMin, int yMin, int xMax, int yMax) const {
      return isOutOfBoundsX(xMin, xMax) || isOutOfBoundsY(yMin, yMax);
    }

    // ── Collision detection ─────────────────────────────────────────────

    /**
     * @brief Attempts to force the x,y location of the shape into the
     * provided rectangular boundary (x, y, width, height)
     *
     * @param x X location of boundary
     * @param y Y location of boundary
     * @param width Width of boundary
     * @param height Height of boundary
     */
    void forceInside(int x, int y, int width, int height) {
      if (getTop() <= y) {
        setY(y);
      } else if (getBottom() >= y + height) {
        setY((y + height) - getHeight() - 1);
      }

      if (getLeft() <= x) {
        setX(x);
      } else if (getRight() >= x + width) {
        setX((x + width) - getWidth() - 1);
      }
    }

    /**
     * @brief Checks to see if the current shape overlaps with the provided shape.
     * Overlap is defined as any pixel between this shape touching (equaling)
     * the passed in shape.
     *
     * This function is useful for collision detection between objects.
     *
     * While you can use this parent class function, it is recommended that
     * sub-classes override this function to provide more precise overlap detection
     * as this method is a simple bounding box check.
     *
     * @param shape The shape to check for overlap
     * @return true If the shape overlaps
     * @return false If the shape does not overlap
     */
    virtual bool overlaps(const Shape& shape) const {
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
    virtual bool contains(int x, int y) const {
      return x >= _x &&           // check within left edge
             x <= (_x + _width) &&  // check within right edge
             y >= _y &&           // check within top edge
             y <= (_y + _height);   // check within bottom edge
    }

    // ── Identity & debugging ────────────────────────────────────────────

    /**
     * @brief Returns the type of this shape as a ShapeType enum value.
     *
     * Used internally by overlaps() for fast, allocation-free type checking
     * to select the best collision algorithm (e.g., radial overlap for circles).
     * Sub-classes should override this.
     *
     * @return ShapeType
     */
    virtual ShapeType getShapeType() const {
      return SHAPE;
    }

    /**
     * @brief Gets the name of this shape as a C string. Sub-classes should override.
     *
     * Returns a const char* (not an Arduino String) to avoid heap allocation.
     * On memory-constrained boards like the Arduino Uno, returning String by value
     * from a function called in a hot loop (e.g., collision checks at 30+ FPS)
     * causes heap fragmentation that can crash the board.
     *
     * @return const char* A string literal — no allocation, no cleanup needed
     */
    virtual const char* getName() const {
      return "Shape";
    }

    /**
     * @brief Gets a basic toString() for the object. Sub-classes should override
     * to get a more fitting description.
     *
     * Unlike getName(), toString() returns an Arduino String because it builds
     * a formatted string with variable data. This is acceptable since toString()
     * is only used for debugging (e.g., Serial.println), not in performance-critical
     * loops.
     *
     * @return String
     */
    virtual String toString() const {
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
    static float distance(int x1, int y1, int x2, int y2) {
      return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    }
};

// ═══════════════════════════════════════════════════════════════════════
// Rectangle
// ═══════════════════════════════════════════════════════════════════════

/**
 * @brief A Rectangle shape class that extends Shape.
 */
class Rectangle : public Shape {
  public:
    Rectangle(int x, int y, int width, int height)
      : Rectangle(x, y, width, height, false) {
      // purposefully empty
    }

    Rectangle(int x, int y, int width, int height, bool drawFillOn)
      : Shape(x, y, width, height, drawFillOn) {
      // purposefully empty
    }

    /**
     * @brief Draw the rectangle
     * @param display
     */
    void draw(Adafruit_SSD1306& display) const override {
      // Draw rectangle takes in (xTop, yTop, width, height)
      // https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#rectangles-2002784-10
      if (_drawFill) {
        display.fillRect(_x, _y, _width, _height, SSD1306_WHITE);
      } else {
        display.drawRect(_x, _y, _width, _height, SSD1306_WHITE);
      }

      // We don't call the parent class draw() call because
      // the bounding box is simply another rectangle
      // Shape::draw(display);
    }

    ShapeType getShapeType() const override {
      return RECTANGLE;
    }

    const char* getName() const override {
      return "Rectangle";
    }
};

// ═══════════════════════════════════════════════════════════════════════
// RoundRect
// ═══════════════════════════════════════════════════════════════════════

/**
 * @brief A rectangle with rounded corners. Extends Rectangle.
 *
 * Wraps the Adafruit_GFX drawRoundRect() and fillRoundRect() functions.
 * Useful for drawing UI elements like health bars, buttons, or score boxes.
 *
 * @section usage Usage Example
 * @code
 *   RoundRect healthBar(10, 2, 50, 8, 3);  // x, y, width, height, cornerRadius
 *   healthBar.setDrawFill(true);
 *
 *   display.clearDisplay();
 *   healthBar.draw(display);
 *   display.display();
 * @endcode
 */
class RoundRect : public Rectangle {
  protected:
    int _cornerRadius;  ///< Radius of the rounded corners in pixels

  public:
    /**
     * @brief Construct a rounded rectangle.
     *
     * @param x            X position (top-left)
     * @param y            Y position (top-left)
     * @param width        Width in pixels
     * @param height       Height in pixels
     * @param cornerRadius Radius of rounded corners in pixels
     */
    RoundRect(int x, int y, int width, int height, int cornerRadius)
      : RoundRect(x, y, width, height, cornerRadius, false) {
      // purposefully empty
    }

    RoundRect(int x, int y, int width, int height, int cornerRadius, bool drawFillOn)
      : Rectangle(x, y, width, height, drawFillOn) {
      _cornerRadius = cornerRadius;
    }

    /**
     * @brief Get the corner radius
     * @return int Corner radius in pixels
     */
    int getCornerRadius() const {
      return _cornerRadius;
    }

    /**
     * @brief Set the corner radius
     * @param cornerRadius Corner radius in pixels
     */
    void setCornerRadius(int cornerRadius) {
      _cornerRadius = cornerRadius;
    }

    /**
     * @brief Draw the rounded rectangle
     * @param display
     */
    void draw(Adafruit_SSD1306& display) const override {
      // drawRoundRect and fillRoundRect take (x, y, w, h, radius, color)
      // https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#rounded-rectangles-2002790-12
      if (_drawFill) {
        display.fillRoundRect(_x, _y, _width, _height, _cornerRadius, SSD1306_WHITE);
      } else {
        display.drawRoundRect(_x, _y, _width, _height, _cornerRadius, SSD1306_WHITE);
      }
    }

    ShapeType getShapeType() const override {
      return ROUND_RECT;
    }

    const char* getName() const override {
      return "RoundRect";
    }
};

// ═══════════════════════════════════════════════════════════════════════
// Triangle
// ═══════════════════════════════════════════════════════════════════════

/**
 * @brief A triangle defined by three vertices.
 *
 * The bounding box is automatically computed from the three vertices and
 * used for collision detection. The shape's x,y position is the top-left
 * corner of this bounding box.
 *
 * @note Moving a Triangle with setX()/setY()/setLocation()/update() moves
 *       all three vertices by the same offset, preserving the triangle's shape.
 *
 * @section usage Usage Example
 * @code
 *   // An arrow pointing right
 *   Triangle arrow(60, 20, 80, 32, 60, 44);
 *   arrow.setDrawFill(true);
 *   arrow.setSpeed(1, 0); // move right
 *
 *   void loop() {
 *     arrow.update();
 *     display.clearDisplay();
 *     arrow.draw(display);
 *     display.display();
 *   }
 * @endcode
 */
class Triangle : public Shape {
  protected:
    // The three vertices of the triangle, stored as absolute coordinates
    int _x0, _y0;
    int _x1, _y1;
    int _x2, _y2;

  public:
    /**
     * @brief Construct a triangle from three vertices.
     *
     * The bounding box is computed automatically from the min/max of the
     * three vertex coordinates.
     *
     * @param x0 First vertex X
     * @param y0 First vertex Y
     * @param x1 Second vertex X
     * @param y1 Second vertex Y
     * @param x2 Third vertex X
     * @param y2 Third vertex Y
     */
    Triangle(int x0, int y0, int x1, int y1, int x2, int y2)
      : Triangle(x0, y0, x1, y1, x2, y2, false) {
      // purposefully empty
    }

    Triangle(int x0, int y0, int x1, int y1, int x2, int y2, bool drawFillOn)
      : Shape(
          min(x0, min(x1, x2)),             // bounding box x = leftmost vertex
          min(y0, min(y1, y2)),             // bounding box y = topmost vertex
          max(x0, max(x1, x2)) - min(x0, min(x1, x2)),  // width
          max(y0, max(y1, y2)) - min(y0, min(y1, y2)),   // height
          drawFillOn
        )
    {
      _x0 = x0;
      _y0 = y0;
      _x1 = x1;
      _y1 = y1;
      _x2 = x2;
      _y2 = y2;
    }

    /**
     * @brief Draw the triangle
     * @param display
     */
    void draw(Adafruit_SSD1306& display) const override {
      // drawTriangle and fillTriangle take three (x,y) vertex pairs
      // https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#triangles-2002786-8
      if (_drawFill) {
        display.fillTriangle(_x0, _y0, _x1, _y1, _x2, _y2, SSD1306_WHITE);
      } else {
        display.drawTriangle(_x0, _y0, _x1, _y1, _x2, _y2, SSD1306_WHITE);
      }

      // Call super method (draws bounding box if enabled)
      Shape::draw(display);
    }

    /**
     * @brief Sets the x location. Moves all three vertices by the same offset
     * so the triangle's shape is preserved.
     * @param x The new x location (top-left of bounding box)
     */
    void setX(int x) override {
      int dx = x - _x;
      _x0 += dx;
      _x1 += dx;
      _x2 += dx;
      _x = x;
    }

    /**
     * @brief Sets the y location. Moves all three vertices by the same offset
     * so the triangle's shape is preserved.
     * @param y The new y location (top-left of bounding box)
     */
    void setY(int y) override {
      int dy = y - _y;
      _y0 += dy;
      _y1 += dy;
      _y2 += dy;
      _y = y;
    }

    /**
     * @brief Get the first vertex X coordinate
     * @return int
     */
    int getX0() const { return _x0; }

    /**
     * @brief Get the first vertex Y coordinate
     * @return int
     */
    int getY0() const { return _y0; }

    /**
     * @brief Get the second vertex X coordinate
     * @return int
     */
    int getX1() const { return _x1; }

    /**
     * @brief Get the second vertex Y coordinate
     * @return int
     */
    int getY1() const { return _y1; }

    /**
     * @brief Get the third vertex X coordinate
     * @return int
     */
    int getX2() const { return _x2; }

    /**
     * @brief Get the third vertex Y coordinate
     * @return int
     */
    int getY2() const { return _y2; }

    ShapeType getShapeType() const override {
      return TRIANGLE;
    }

    const char* getName() const override {
      return "Triangle";
    }

    String toString() const override {
      return (String)"v0:(" + _x0 + "," + _y0 + ") v1:(" +
             _x1 + "," + _y1 + ") v2:(" + _x2 + "," + _y2 + ")";
    }
};

// ═══════════════════════════════════════════════════════════════════════
// Circle
// ═══════════════════════════════════════════════════════════════════════

/**
 * @brief A circular shape class that extends Shape.
 *
 * Internally stored as a bounding box; the constructor converts
 * (xCenter, yCenter, radius) to top-left coordinates. The bounding box
 * size is (2 * radius + 1) to account for the center pixel.
 */
class Circle : public Shape {

  public:
    Circle(int xCenter, int yCenter, int radius)
      : Circle(xCenter, yCenter, radius, false) {
      // purposefully empty
    }

    Circle(int xCenter, int yCenter, int radius, bool drawFillOn)
      : Shape(xCenter - radius, yCenter - radius, (radius * 2) + 1, (radius * 2) + 1, drawFillOn) {
      // purposefully empty
    }

    /**
     * @brief Draw the circle
     * @param disp
     */
    void draw(Adafruit_SSD1306& disp) const override {
      // Draw circle takes in (xCenter, yCenter, radius)
      // https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#circles-2002788-14
      int radius = getRadius();
      if (_drawFill) {
        disp.fillCircle(_x + radius, _y + radius, radius, SSD1306_WHITE);
      } else {
        disp.drawCircle(_x + radius, _y + radius, radius, SSD1306_WHITE);
      }

      // Call super method (draws bounding box if enabled)
      Shape::draw(disp);
    }

    /**
     * @brief Calculates the overlap between two circular objects.
     * Uses distance between center points compared to sum of radii,
     * which is more precise than bounding-box overlap for circles.
     *
     * @param circle
     * @return true
     * @return false
     */
    bool overlaps(const Circle& circle) const {
      float distanceFromCenterPoints = Shape::distance(getCenterX(), getCenterY(),
                                                       circle.getCenterX(), circle.getCenterY());
      return distanceFromCenterPoints <= getRadius() + circle.getRadius();
    }

    /**
     * @brief Calculates the overlap between two shapes.
     * If both shapes are circles (including Ball, which reports CIRCLE),
     * uses the more precise radius-based overlap. Otherwise falls back
     * to the parent bounding-box overlap function.
     *
     * @param shape
     * @return true
     * @return false
     */
    bool overlaps(const Shape& shape) const override {
      // Use ShapeType enum instead of string comparison to avoid
      // heap-allocating Arduino Strings on every collision check.
      // Ball returns CIRCLE for its ShapeType, so Ball-to-Circle and
      // Ball-to-Ball collisions correctly use radial overlap.
      if (shape.getShapeType() == CIRCLE) {
        return this->overlaps((const Circle&)shape);
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
    void setDimensions(int width, int height) override {
      Shape::setDimensions(width, width);
    }

    /**
     * @brief Moves the location of the object so that the center is at x,y
     *
     * @param x
     * @param y
     */
    void setCenter(int x, int y) {
      int radius = getRadius();
      setLocation(x - radius, y - radius);
    }

    /**
     * @brief Get the radius of the circle
     * @return int
     */
    int getRadius() const {
      return getWidth() / 2;
    }

    /**
     * @brief Set the radius of the circle. Updates the bounding box to
     * (2 * radius + 1) to match the constructor and Adafruit_GFX convention
     * of accounting for the center pixel.
     *
     * @param radius
     */
    void setRadius(int radius) {
      // Must use (2 * radius) + 1 to match the constructor, which also
      // adds 1 for the center pixel. Without this, the bounding box would
      // shrink by a pixel when students dynamically change a circle's size.
      int size = (2 * radius) + 1;
      setDimensions(size, size);
    }

    ShapeType getShapeType() const override {
      return CIRCLE;
    }

    /**
     * @brief Get the name of the circle
     * @return const char*
     */
    const char* getName() const override {
      return "Circle";
    }
};

// ═══════════════════════════════════════════════════════════════════════
// Ball (backwards compatibility)
// ═══════════════════════════════════════════════════════════════════════

/**
 * @brief A convenience subclass of Circle, kept for backwards compatibility.
 *
 * In earlier versions of this library, Ball was the only shape with velocity
 * (setSpeed, update, reverseXSpeed, reverseYSpeed) and boundary checking.
 * These features have since been moved to the Shape base class so that
 * any shape can move — rectangles, triangles, etc.
 *
 * Ball still works exactly as before. Existing code using Ball does not
 * need to change. For new code, you can use Circle (or any shape) directly
 * with setSpeed() and update().
 *
 * checkXBounce() and checkYBounce() are kept for backwards compatibility;
 * new code should use isOutOfBoundsX() and isOutOfBoundsY() instead, which
 * are available on all shapes.
 */
class Ball : public Circle {

  public:
    Ball(int xCenter, int yCenter, int radius) : Circle(xCenter, yCenter, radius) {
      // purposefully empty
    }

    /**
     * @brief Check if the ball has reached a vertical boundary.
     * @deprecated Use isOutOfBoundsY() instead, which is available on all shapes.
     */
    boolean checkYBounce(int yMin, int yMax) const {
      return isOutOfBoundsY(yMin, yMax);
    }

    /**
     * @brief Check if the ball has reached a horizontal boundary.
     * @deprecated Use isOutOfBoundsX() instead, which is available on all shapes.
     */
    boolean checkXBounce(int xMin, int xMax) const {
      return isOutOfBoundsX(xMin, xMax);
    }

    // Ball reports CIRCLE for ShapeType so that Circle::overlaps()
    // correctly uses radial collision for Ball-to-Circle and
    // Ball-to-Ball overlap checks
    ShapeType getShapeType() const override {
      return CIRCLE;
    }

    const char* getName() const override {
      return "Ball";
    }
};
