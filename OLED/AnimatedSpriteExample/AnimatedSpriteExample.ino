/**
 * Animates a sprite using the Shape.hpp drawing infrastructure
 * 
 * To create the byte dumps of the images, I used the image2cpp tool here: 
 * http://javl.github.io/image2cpp/
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Shape.hpp>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int LOGO_WIDTH = 50;
const int LOGO_HEIGHT = 35;

// 'JonClosedMouth_15x33-Cropped', 15x33px
const unsigned char PROGMEM _munchSpriteFrame1 []  = {
  0x80, 0x00, 0xff, 0x00, 0xff, 0xc0, 0xff, 0xf0, 0xff, 0xf0, 0xff, 0xf8, 0xff, 0xfc, 0xff, 0xfc, 
  0xff, 0xfc, 0xff, 0xf8, 0xff, 0xf0, 0xff, 0xf0, 0xff, 0xf0, 0xff, 0xf0, 0xff, 0xf8, 0xff, 0xfc, 
  0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfc, 0xff, 0xfc, 0xff, 0xfc, 0xff, 0xfc, 0xff, 0xfe, 0xff, 0xfc, 
  0xff, 0xfc, 0xff, 0xfc, 0xff, 0xfc, 0xff, 0xf8, 0xff, 0xf0, 0xff, 0xc0, 0xff, 0x00, 0xfe, 0x00, 
  0xfe, 0x00
};

// 'JonOpenMouth_15x33-Cropped-BW', 15x33px
const unsigned char PROGMEM _munchSpriteFrame2 []  = {
  0x80, 0x00, 0xfe, 0x00, 0xff, 0xc0, 0xff, 0xe0, 0xff, 0xf0, 0xff, 0xf8, 0xff, 0xf8, 0xff, 0xfc, 
  0xff, 0xf8, 0xff, 0xf0, 0xff, 0xf0, 0xff, 0xf0, 0xff, 0xf0, 0xff, 0xf8, 0xff, 0xfc, 0xff, 0xfc, 
  0xff, 0xfe, 0xff, 0xfc, 0xff, 0xf8, 0xff, 0xf8, 0xff, 0xf8, 0xff, 0xf8, 0xff, 0xf0, 0xff, 0xc0, 
  0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xe0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0x80, 
  0xfe, 0x00
};

// const unsigned char *const _munchSpriteFrames[] PROGMEM = {_munchSpriteFrame1, _munchSpriteFrame2};

class Sprite : public Shape{
   protected:
    //unsigned char **_sprites;
    
    int _numFrames = 2;
    int _curFrameIndex = 0;

    // To read about program memory PROGMEM: 
    // https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
    const unsigned char *_frame1 PROGMEM;
    const unsigned char *_frame2 PROGMEM;
    
    //const unsigned char **_sprites PROGMEM; // couldn't get this to work
  
   public:
    Sprite(const unsigned char *frame1, const unsigned char *frame2, int x, int y, int width, int height)
      : Shape(x, y, width, height, false)
    {
      _frame1 = frame1;
      _frame2 = frame2;
      //_sprites = sprites;
    }

    /**
     * @brief Draw the sprite
     * 
     * @param display 
     */
    void draw (Adafruit_SSD1306& display) override{
      // Draw sprite frames
      if(_curFrameIndex == 0){
        _display.drawBitmap(_x, _y,  _frame1, _width, _height, WHITE);
      }else{
        _display.drawBitmap(_x, _y,  _frame2, _width, _height, WHITE);
      }
      
      _curFrameIndex++;

      if(_curFrameIndex >= _numFrames){
        _curFrameIndex = 0;
      }
    }

    String getName() const override{
      return "Sprite";
    }
};


Sprite _sprite(_munchSpriteFrame1, _munchSpriteFrame2, 20, 20, 15, 33);

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  _display.clearDisplay();
}

void loop() {
  _display.clearDisplay();

  _sprite.draw(_display);

  _display.display();
  
  delay(100); 
}
