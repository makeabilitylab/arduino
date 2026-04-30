/**
 * Simple Chrome Dino Jump Game for SSD1306 OLED (128x64)
 * 
 * Press the button to jump over obstacles. If you hit one, game over!
 * Press the button again to restart.
 *
 * Hardware:
 *  - SSD1306 128x64 I2C OLED
 *  - Momentary push button on BUTTON_PIN (default: pin 2) with INPUT_PULLUP
 *
 * Sprite sheet from:
 * https://www.spriters-resource.com/browser_games/googledinosaurrungame/sheet/78171/
 *
 * Converted to byte arrays:
 * http://javl.github.io/image2cpp/
 *
 * Based on DinoSpriteAnimationDemo by Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Sprites.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Button ---
const int BUTTON_PIN = 12;

// --- Ground ---
const int GROUND_Y = SCREEN_HEIGHT - 1; // y position of ground line

// --- Dino sprites ---
// Use the child dino (20x21px) — frames 3 & 5 have distinct leg poses
const unsigned char* runSprites[2] = { epd_bitmap_child_dino3, epd_bitmap_child_dino5 };

const int SPRITE_WIDTH = CHILD_SPRITE_WIDTH;   // 20
const int SPRITE_HEIGHT = CHILD_SPRITE_HEIGHT;  // 21

// --- Dino state ---
const int DINO_X = 10;                       // Dino stays at fixed x position
int _dinoY;                                  // current y (top-left of sprite)
float _velocityY = 0;                        // jump velocity (negative = up)
const float GRAVITY = 1.2;                   // pixels/frame^2
const float JUMP_VELOCITY = -8.0;            // initial jump impulse
bool _isJumping = false;
int _dinoGroundY;                            // y when standing on ground
int _runFrame = 0;                           // toggles 0/1 for leg animation

// --- Obstacle state ---
const int OBSTACLE_WIDTH = 8;
const int OBSTACLE_HEIGHT = 12;
int _obstacleX;                              // current x position
const int OBSTACLE_SPEED = 4;                // pixels per frame
int _obstacleY;                              // top-left y (sits on ground)

// --- Game state ---
int _score = 0;
bool _gameOver = false;

// --- Timing ---
unsigned long _lastFrameMs = 0;
const int FRAME_DELAY_MS = 30;               // ~33 fps

// --- Animation timing ---
unsigned long _lastAnimMs = 0;
const int ANIM_DELAY_MS = 100;               // leg toggle speed

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.display();

  resetGame();
}

void resetGame() {
  // Dino stands so its feet touch the ground line
  _dinoGroundY = GROUND_Y - SPRITE_HEIGHT;
  _dinoY = _dinoGroundY;
  _velocityY = 0;
  _isJumping = false;
  _runFrame = 0;

  // Obstacle starts off-screen to the right
  _obstacleX = SCREEN_WIDTH;
  _obstacleY = GROUND_Y - OBSTACLE_HEIGHT;

  _score = 0;
  _gameOver = false;
}

void loop() {
  // Frame rate control
  unsigned long now = millis();
  if (now - _lastFrameMs < FRAME_DELAY_MS) {
    return;
  }
  _lastFrameMs = now;

  // Read button (active LOW with INPUT_PULLUP)
  bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);

  if (_gameOver) {
    // Wait for button press to restart
    if (buttonPressed) {
      resetGame();
    }
    return; // Don't redraw while waiting
  }

  // Jump dino, jump!
  if (buttonPressed && !_isJumping) {
    _velocityY = JUMP_VELOCITY;
    _isJumping = true;
  }

  // Apply gravity
  _velocityY += GRAVITY;
  _dinoY += (int)_velocityY;

  // Land on ground
  if (_dinoY >= _dinoGroundY) {
    _dinoY = _dinoGroundY;
    _velocityY = 0;
    _isJumping = false;
  }

  // Move obstacle
  _obstacleX -= OBSTACLE_SPEED;
  if (_obstacleX + OBSTACLE_WIDTH < 0) {
    // Obstacle passed off-screen: score a point, reset it
    _obstacleX = SCREEN_WIDTH;
    _score++;
  }

  // Collision detection (AABB)
  // Use a slightly trimmed bounding box for the dino
  int dinoHitX = DINO_X + 3;
  int dinoHitY = _dinoY + 2;
  int dinoHitW = SPRITE_WIDTH - 6;
  int dinoHitH = SPRITE_HEIGHT - 2;

  bool collided = (dinoHitX < _obstacleX + OBSTACLE_WIDTH &&
                   dinoHitX + dinoHitW > _obstacleX &&
                   dinoHitY < _obstacleY + OBSTACLE_HEIGHT &&
                   dinoHitY + dinoHitH > _obstacleY);

  if (collided) {
    _gameOver = true;
    drawGameOver();
    return;
  }

  // Animate legs (only when running on ground)
  if (!_isJumping && now - _lastAnimMs >= ANIM_DELAY_MS) {
    _runFrame = 1 - _runFrame; // toggle 0 <-> 1
    _lastAnimMs = now;
  }

  // --- Draw ---

  // Clear the display to prepare next frame
  display.clearDisplay();

  // Ground line
  display.drawLine(0, GROUND_Y, SCREEN_WIDTH - 1, GROUND_Y, WHITE);

  // Dino sprite (use frame 0 when jumping for a static pose)
  int frame = _isJumping ? 0 : _runFrame;
  display.drawBitmap(DINO_X, _dinoY, runSprites[frame], SPRITE_WIDTH, SPRITE_HEIGHT, WHITE);

  // Draw obstacle
  display.fillRect(_obstacleX, _obstacleY, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, WHITE);

  // Draw score
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(SCREEN_WIDTH - 30, 2);
  display.print(_score);

  display.display();
}

void drawGameOver() {
  display.clearDisplay();

  // Ground line
  display.drawLine(0, GROUND_Y, SCREEN_WIDTH - 1, GROUND_Y, WHITE);

  // Dino at current position
  display.drawBitmap(DINO_X, _dinoY, runSprites[0], SPRITE_WIDTH, SPRITE_HEIGHT, WHITE);

  // Obstacle at current position
  display.fillRect(_obstacleX, _obstacleY, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, WHITE);

  // Game over text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 2);
  display.print(F("GAME OVER"));

  display.setCursor(20, 14);
  display.print(F("Score: "));
  display.print(_score);

  display.display();
}
