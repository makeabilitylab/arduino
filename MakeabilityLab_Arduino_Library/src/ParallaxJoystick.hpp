/**
 * @file ParallaxJoystick.hpp
 * @brief A convenience class to work with the Parallax 2-axis joystick.
 *
 * Handles analog reads from two pins and maps them to up/down and
 * left/right values, with support for different physical orientations.
 *
 * See: https://www.parallax.com/product/2-axis-joystick/
 *
 * @section usage Usage Example
 * @code
 *   // Basic usage — Y-axis facing up (default)
 *   ParallaxJoystick joystick(A0, A1);
 *
 *   // With orientation and 12-bit ADC (e.g., ESP32)
 *   ParallaxJoystick joystick(A0, A1, 4095, RIGHT);
 *
 *   void loop() {
 *     joystick.read();
 *     int upDown    = joystick.getUpDownVal();     // 0 to maxAnalogValue
 *     int leftRight = joystick.getLeftRightVal();  // 0 to maxAnalogValue
 *   }
 * @endcode
 *
 * @author Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/
 */
#pragma once

// The joystick's orientation with respect to the user.
// We need this because sometimes we have to place a joystick
// upside down, etc. in our designs
enum JoystickYDirection {
  UP,     ///< Y-axis label faces up (default, no remapping)
  RIGHT,  ///< Y-axis label faces right
  DOWN,   ///< Y-axis label faces down (both axes inverted)
  LEFT    ///< Y-axis label faces left
};

class ParallaxJoystick {

  protected:
    static const int DEFAULT_MAX_ANALOG_VALUE = 1023;

    const int _upDownInputPin;      ///< Analog pin for the up/down axis
    const int _leftRightInputPin;   ///< Analog pin for the left/right axis

    int _maxAnalogValue = DEFAULT_MAX_ANALOG_VALUE;
    int _joyStickCenterValue = int(DEFAULT_MAX_ANALOG_VALUE / 2);
    enum JoystickYDirection _joystickYDir = UP;

    int _upDownVal = 0;       ///< Most recent up/down reading (after orientation mapping)
    int _leftRightVal = 0;    ///< Most recent left/right reading (after orientation mapping)

  public:
    // The colon syntax below is used to initialize const members
    // (const members cannot be assigned in the constructor body)
    ParallaxJoystick(int upDownInputPin, int leftRightInputPin)
      : _upDownInputPin(upDownInputPin), _leftRightInputPin(leftRightInputPin) {
      // purposefully empty
    }

    // Uses C++ 11 delegating constructor syntax to call the 2-arg constructor
    ParallaxJoystick(int upDownInputPin, int leftRightInputPin,
                     int maxAnalogValue, JoystickYDirection joystickYDir)
      : ParallaxJoystick(upDownInputPin, leftRightInputPin) {
      _joystickYDir = joystickYDir;
      setMaxAnalogValue(maxAnalogValue);
    }

    /**
     * @brief Get the up-down Joystick input pin
     * @return int
     */
    int getUpDownPin() const { return _upDownInputPin; }

    /**
     * @brief Get the left-right Joystick input pin
     * @return int
     */
    int getLeftRightPin() const { return _leftRightInputPin; }

    /**
     * @brief Get the max analog value (e.g., 1023 for Arduino Uno and Leonardo
     * with 10-bit ADCs and 4095 for 12-bit ADCs)
     * @return int
     */
    int getMaxAnalogValue() const { return _maxAnalogValue; }

    /**
     * @brief Get the center of the joystick range
     * @return int
     */
    int getCenter() const { return _joyStickCenterValue; }

    /**
     * @brief Set the maximum analog value
     * @param maxAnalogVal
     */
    void setMaxAnalogValue(int maxAnalogVal) {
      _maxAnalogValue = maxAnalogVal;
      _joyStickCenterValue = int(maxAnalogVal / 2);
    }

    /**
     * @brief Set the joystick Y orientation. Defaults to UP.
     */
    void setJoystickYOrientation(JoystickYDirection joystickYDir) {
      _joystickYDir = joystickYDir;
    }

    /**
     * @brief Gets a value between 0 and _maxAnalogValue (1023 on Arduino Uno)
     * that represents up-down joystick direction.
     * @return int
     */
    int getUpDownVal() const { return _upDownVal; }

    /**
     * @brief Gets a value between 0 and _maxAnalogValue (1023 on Arduino Uno)
     * that represents left-right joystick direction.
     * @return int
     */
    int getLeftRightVal() const { return _leftRightVal; }

    /**
     * @brief Read the joystick values. Call getUpDownVal() and getLeftRightVal() to retrieve.
     *
     * If hooked up on the breadboard, we often have to install the joystick in a
     * different orientation than with the Y up direction facing up. The code below
     * handles the different orientations by remapping which raw axis maps to
     * up/down vs left/right and inverting as needed.
     */
    void read() {
      int joystickUpDownVal = analogRead(_upDownInputPin);
      int joystickLeftRightVal = analogRead(_leftRightInputPin);

      // Remap axes based on how the joystick is physically oriented
      if (_joystickYDir == RIGHT) {
        int tmpX = joystickLeftRightVal;
        _leftRightVal = _maxAnalogValue - joystickUpDownVal;
        _upDownVal = tmpX;
      } else if (_joystickYDir == DOWN) {
        _upDownVal = _maxAnalogValue - joystickUpDownVal;
        _leftRightVal = _maxAnalogValue - joystickLeftRightVal;
      } else if (_joystickYDir == LEFT) {
        int tmpX = joystickLeftRightVal;
        _leftRightVal = joystickUpDownVal;
        _upDownVal = _maxAnalogValue - tmpX;
      } else {
        _leftRightVal = joystickLeftRightVal;
        _upDownVal = joystickUpDownVal;
      }
    }
};
