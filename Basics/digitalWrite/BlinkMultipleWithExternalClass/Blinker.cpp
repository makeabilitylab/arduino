
#include "Blinker.h"

Blinker::Blinker(const int pin, const unsigned long blinkInterval) :
  _pin(pin), _interval(blinkInterval) // initialize const like this in C++
{
  _state = LOW;
  _lastToggledTimestamp = 0;
  pinMode(_pin, OUTPUT);
}

/**
* Calculates whether to toggle output state based on the set interval
* Call this function once per loop()
*/
void Blinker::update() {
  unsigned long currentTimestampMs = millis();

  if (currentTimestampMs - _lastToggledTimestamp >= _interval) {
    _lastToggledTimestamp = currentTimestampMs;
    _state = !_state;
    digitalWrite(_pin, _state);
  }
}
