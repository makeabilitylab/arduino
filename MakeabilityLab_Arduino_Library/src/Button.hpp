/**
 * @file Button.hpp
 * @brief Debounced digital button input with press/release detection.
 *
 * Provides a simple Button class that handles debouncing internally,
 * supports both active-low (with internal pull-up) and active-high wiring,
 * and exposes press, release, and state-change queries.
 *
 * There are lots of wonderful button classes for Arduino. For a relatively recent
 * breakdown, see this Stack Overflow post: https://arduino.stackexchange.com/a/80857/63793.
 * However, all of these libraries are external. Sometimes, I just want to rely on my own code
 * and import as few libraries as possible.
 *
 * A few well-designed libraries that I also took inspiration from:
 *  - https://github.com/avandalen/avdweb_Switch
 *  - https://github.com/evert-arias/EasyButton
 *  - https://github.com/JChristensen/JC_Button
 *
 * @section usage Usage Example
 * @code
 *   Button myButton(2);  // pin 2, 25ms debounce, internal pull-up, active-low
 *
 *   void setup() {
 *     myButton.begin();   // must call before read()
 *   }
 *
 *   void loop() {
 *     myButton.read();
 *     if (myButton.isPressed())  { /* held down   */ }
 *     if (myButton.wasPressed()) { /* just released */ }
 *   }
 * @endcode
 *
 * @author Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/
 */
#pragma once

class Button {
  protected:
    uint8_t _pin;                         ///< The digital input pin connected to the button
    bool _isInternalPullUpEnabled;        ///< If true, INPUT_PULLUP is used in begin()
    bool _isActiveLow;                    ///< If true, LOW means pressed (typical with pull-ups)
    bool _stateChange;                    ///< True if the state changed on the most recent read()
    bool _curState;                       ///< Current debounced state: true == pressed
    bool _prevState;                      ///< Previous debounced state
    unsigned long _lastReadTimestamp;     ///< millis() timestamp of the most recent read()
    unsigned long _lastChangeTimestamp;   ///< millis() timestamp of the last state change
    unsigned long _debounceTime;          ///< Debounce window in milliseconds

  public:
    /**
     * @brief Construct a new Button.
     *
     * @param buttonPin              Digital pin connected to the button
     * @param debounceTime           Debounce window in ms (default 25)
     * @param internalPullUpEnabled  Enable INPUT_PULLUP (default true)
     * @param activeLow              If true, LOW == pressed (default true)
     */
    Button(uint8_t buttonPin, unsigned long debounceTime = 25,
           bool internalPullUpEnabled = true, bool activeLow = true)
      : _pin(buttonPin),
        _debounceTime(debounceTime),
        _isInternalPullUpEnabled(internalPullUpEnabled),
        _isActiveLow(activeLow),
        _stateChange(false),
        _curState(false),
        _prevState(false),
        _lastReadTimestamp(0),
        _lastChangeTimestamp(0)
    {}

    /**
     * @brief Initialize the button pin and read the initial state.
     *        You must call begin() before read() — typically in setup().
     */
    void begin() {
      pinMode(_pin, _isInternalPullUpEnabled ? INPUT_PULLUP : INPUT);

      unsigned long curTimestampMs = millis();
      _curState = digitalRead(_pin);

      // If active low, invert the state so that _curState == true means "pressed"
      // (regardless of the wiring). This normalization makes all the query methods
      // (isPressed, wasPressed, etc.) work in terms of "pressed" vs "not pressed"
      // without the caller needing to think about LOW vs HIGH.
      if (_isActiveLow) {
        _curState = !_curState;
      }

      _lastReadTimestamp = curTimestampMs;
      _lastChangeTimestamp = curTimestampMs;
      _prevState = _curState;
      _stateChange = false;
    }

    /**
     * @brief Read the pin and update the debounced button state.
     *
     * Call this once per loop() iteration. After calling, use isPressed(),
     * wasPressed(), and didStateChange() to query the result.
     *
     * @return true if the button is currently pressed (after debouncing)
     */
    bool read() {
      unsigned long curTimestampMs = millis();
      bool pinVal = digitalRead(_pin);

      if (_isActiveLow) {
        pinVal = !pinVal;
      }

      if (curTimestampMs - _lastChangeTimestamp < _debounceTime) {
        // Still within the debounce window — ignore any changes to avoid
        // registering spurious transitions from mechanical switch bounce
        _stateChange = false;
      } else {
        _prevState = _curState;
        _curState = pinVal;
        _stateChange = (_curState != _prevState);
        if (_stateChange) {
          _lastChangeTimestamp = curTimestampMs;
        }
      }

      _lastReadTimestamp = curTimestampMs;
      return _curState;
    }

    /**
     * @brief Returns true if button state is currently pressed. Does not call read().
     * @return true if the button is held down
     */
    bool isPressed() const {
      return _curState;
    }

    /**
     * @brief Returns true if the button was just pressed but is now released.
     *        Does not call read().
     * @return true on the read() cycle immediately after the button is released
     */
    bool wasPressed() const {
      return _prevState && !_curState && _stateChange;
    }

    /**
     * @brief Returns true if the button state changed between reads.
     * @return true if the state transitioned between pressed and released
     */
    bool didStateChange() const {
      return _stateChange;
    }

    /**
     * @brief Returns the time in milliseconds since the last button state changed.
     * @return Milliseconds since the last press or release transition
     */
    unsigned long timeSinceLastStateChange() const {
      return millis() - _lastChangeTimestamp;
    }

    /**
     * @brief The time in milliseconds (from millis()) of the last state change.
     * @return Timestamp in milliseconds
     */
    unsigned long lastStateChangeTimestamp() const {
      return _lastChangeTimestamp;
    }
};
