/**
 * @brief Provides an easy-to-use wrapper around ESP32 tone functionality.
 * 
 * ## BACKGROUND ##
 * The Arduino tone() function relies on timer interrupts to precisely control the frequency 
 * of a PWM waveform:
 * https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Tone.cpp
 * 
 * This is completely different on the ESP32. So, we need a different approach. Fortunately,
 * there is some basic infrastructure in the arduino-esp32 library to help. Specifically,
 * in esp32-hal-ledc:
 * https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-ledc.h
 * https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-ledc.c
 * 
 * Here, there are a few things of interest. First:
 *   typedef enum {
 *     NOTE_C, NOTE_Cs, NOTE_D, NOTE_Eb, NOTE_E, NOTE_F, NOTE_Fs, NOTE_G, NOTE_Gs, NOTE_A, NOTE_Bb, NOTE_B, NOTE_MAX
 *  } note_t;
 * 
 * Which just nicely sets up named indices into the method:
 *   ledcWriteNote(uint8_t chan, note_t note, uint8_t octave){ 
 * 
 * So, we can call ledcWriteNote(<pwm_channel>, NOTE_C, 4); to play middle C (C4)
 * 
 * We can also just call ledcWriteTone(uint8_t chan, double freq) to play a raw
 * frequency at that PWM channel.
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/
 */
class Tone32{

  protected:
    uint8_t  _outputPin;               // output pin
    uint8_t  _pwmChannel;              // pwm channel
    unsigned long _playDuration = 0;   // play duration in ms
    unsigned long _playStarted = 0;    // play start time in ms

    bool _pinAttached = false;

  public:

    /**
     * @brief Construct a new Tone32 object
     * 
     * @param outputPin The pin connected to your piezo buzzer
     * @param pwmChannel The PWM channel you want to use: ESP32 supports 0 - 15
     */
    Tone32(uint8_t outputPin, uint8_t pwmChannel){
        setupOutputs(outputPin, pwmChannel);
    }

    /**
     * @brief Automatically called by constructor when creating a new object.
     * If you want to switch output pins or channels dynamically, call this
     * 
    * @param outputPin The pin connected to your piezo buzzer
     * @param pwmChannel The PWM channel you want to use: ESP32 supports 0 - 15
     */
    void setupOutputs(uint8_t outputPin, uint8_t pwmChannel){
      if(_pinAttached){
        ledcDetachPin(_outputPin);
      }
      _outputPin = outputPin;
      _pwmChannel = pwmChannel;
      ledcAttachPin(_outputPin, _pwmChannel);

      _pinAttached = true;
    }

    /**
     * @brief Gets the output pin
     * 
     * @return uint8_t the output pin
     */
    uint8_t getOutputPin() const{
      return _outputPin;
    }

    /**
     * @brief Get the PWM channel
     * 
     * @return uint8_t the PWM channel
     */
    uint8_t getPwmChannel() const{
      return _pwmChannel;
    }

    /**
     * @brief Gets the current play duration in ms. If not set, returns 0
     * 
     * @return unsigned long 
     */
    unsigned long getPlayDuration() const{
        return _playDuration;
    }

    /**
     * @brief Gets the current play duration remaining in ms. If not set, returns 0
     * 
     * @return unsigned long 
     */
    unsigned long getPlayDurationRemaining() const{
        if(isPlaying()){
            return millis() - _playStarted;        
        }
        return 0;
    }

    /**
     * @brief Returns true if currently playing.
     * 
     * @return true if currently playing
     * @return false if not currently playing
     */
    bool isPlaying() const{
      return _playStarted != 0;
    }

    /**
     * @brief Stops playing
     */
    void stopPlaying(){
      // To turn off tone, write out a PWM waveform with 0 frequency
      ledcWrite(_pwmChannel, 0);
      _playStarted = 0;
      _playDuration = 0;
    }

    /**
     * @brief Plays current note_t at the given octave
     *        note_t is defined here: 
     *        https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-ledc.h
     * @param note the note to play
     * @param octave must be between 0 and 8
     */
    void playNote(note_t note, uint8_t octave){
      _playStarted = millis();
      ledcWriteNote(_pwmChannel, note, octave);
    }

    /**
     * @brief Plays current note_t at the given octave for the given duration in milliseconds
     *        For this to work, you must call update() on every loop() call
     * 
     * @param note the note to play
     * @param octave must be between 0 and 8
     * @param duration play duration in milliseconds
     */
    void playNote(note_t note, uint8_t octave, unsigned long duration){
      playNote(note, octave);
      _playDuration = duration;
    }

    /**
     * @brief Plays the given frequency. To stop, call stopPlaying()
     * 
     * @param freq 
     */
    void playTone(double freq){
      // The ledcWriteTone signature: double ledcWriteTone(uint8_t chan, double freq)
      // See: https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-ledc.c
      _playStarted = millis();
      ledcWriteTone(_pwmChannel, freq);
    }

    /**
     * @brief Plays the given frequency for the given duration in milliseconds
     * 
     * @param freq 
     * @param duration play duration in milliseconds
     */
    void playTone(double freq, unsigned long duration){
      playTone(freq);
      _playDuration = duration;
    }

    /**
     * @brief For any of the duration parameters to work, you must call update()
     *        on every loop()
     */
    void update(){
      unsigned long currentTimestampMs = millis();
      if(isPlaying() && millis() - _playStarted >= _playDuration){
        stopPlaying();
      } 
    }
};