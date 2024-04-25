/**
 * Animates the Chrome dinosaur from the dinosaur game
 * Shows both "mother" dino and "child" dino
 * 
 * Sprite sheet from:
 * https://www.spriters-resource.com/browser_games/googledinosaurrungame/sheet/78171/
 *
 * Converted to byte arrays:
 * http://javl.github.io/image2cpp/
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

// #include <SPI.h> Uncomment if you use SPI
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Sprites.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


int _motherDinoSpriteIndex = 0;
int _xMotherSprite = 0; 
int _yMotherSprite = 0; 
int _xMotherSpeed = 2; // in pixels per frame

int _childDinoSpriteIndex = 0;
int _xChildSprite = 0; 
int _yChildSprite = 0;
int _xChildSpeed = 1; // in pixels per frame

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  _display.clearDisplay();

  // Setup x and y locations of sprites
  _xMotherSprite = 2;
  _yMotherSprite = _display.height() / 2 - MOTHER_SPRITE_HEIGHT / 2;

  _xChildSprite = _xMotherSprite + MOTHER_SPRITE_WIDTH + 15;
  _yChildSprite = _yMotherSprite + MOTHER_SPRITE_HEIGHT - CHILD_SPRITE_HEIGHT;
  
}

void loop() {
  _display.clearDisplay();

  // Draw mother sprite
  _display.drawBitmap(_xMotherSprite, _yMotherSprite,  epd_bitmap_mother_dino_array[_motherDinoSpriteIndex], 
                      MOTHER_SPRITE_WIDTH, MOTHER_SPRITE_HEIGHT, WHITE);  
  
  // Setup next animation index
  _motherDinoSpriteIndex++;
  if(_motherDinoSpriteIndex >= NUM_MOTHER_DINO_SPRITES){
    _motherDinoSpriteIndex = 0;
  }
  
  // Setup movement 
  _xMotherSprite += _xMotherSpeed;
  if(_xMotherSprite > _display.width()){
    _xMotherSprite = -MOTHER_SPRITE_WIDTH;
  }

  // Draw child sprite
  _display.drawBitmap(_xChildSprite, _yChildSprite,  epd_bitmap_child_dino_array[_childDinoSpriteIndex], 
                      CHILD_SPRITE_WIDTH, CHILD_SPRITE_HEIGHT, WHITE);  
  
  // Setup next animation frame
  _childDinoSpriteIndex++;
  if(_childDinoSpriteIndex >= NUM_CHILD_DINO_SPRITES){
    _childDinoSpriteIndex = 0;
  }

  // Setup child movement
  // Setup movement 
  _xChildSprite += _xChildSpeed;
  if(_xChildSprite > _display.width()){
    _xChildSprite = -CHILD_SPRITE_WIDTH;
  }

  _display.display();

  // Change delay here to change animation speed
  // A better way to do this would be to track millis() and switch after every
  // set time allotment
  delay(20);
}
