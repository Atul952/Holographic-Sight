#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13

#define BTN_DIST      2
#define BUTTON_UP     3
#define BUTTON_DOWN   4
#define BUTTON_LEFT   5
#define BUTTON_RIGHT  6

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

int distanceMeters = 35;
const int EEPROM_ADDR = 0;

int centerX = SCREEN_WIDTH / 2;
int centerY = SCREEN_HEIGHT / 2;

unsigned long pressStartDist = 0;
bool distHeld = false;

const unsigned long LONG_PRESS_TIME = 800;
const unsigned long FAST_REPEAT_INTERVAL = 150;

void setup() {
  Serial.begin(9600);

  pinMode(BTN_DIST, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);

  distanceMeters = EEPROM.read(EEPROM_ADDR);
  if (distanceMeters < 0 || distanceMeters > 250) distanceMeters = 35;

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
}

void loop() {
  unsigned long now = millis();

  // --- Distance Control (Single Button) ---
  if (digitalRead(BTN_DIST) == LOW) {
    if (!distHeld) {
      pressStartDist = now;
      distHeld = true;
      distanceMeters += 5;  // Short press: increase
      EEPROM.write(EEPROM_ADDR, distanceMeters);
      delay(200);
    } else if (now - pressStartDist > LONG_PRESS_TIME) {
      static unsigned long lastRepeatDist = 0;
      if (now - lastRepeatDist > FAST_REPEAT_INTERVAL) {
        distanceMeters -= 5;  // Long press: decrease
        if (distanceMeters < 0) distanceMeters = 0;
        EEPROM.write(EEPROM_ADDR, distanceMeters);
        lastRepeatDist = now;
      }
    }
  } else {
    distHeld = false;
  }

  // --- Crosshair Movement (Single Press Only) ---
  if (digitalRead(BUTTON_UP) == LOW) {
    centerY -= 5;
    delay(200);
  }

  if (digitalRead(BUTTON_DOWN) == LOW) {
    centerY += 5;
    delay(200);
  }

  if (digitalRead(BUTTON_LEFT) == LOW) {
    centerX -= 5;
    delay(200);
  }

  if (digitalRead(BUTTON_RIGHT) == LOW) {
    centerX += 5;
    delay(200);
  }

  centerX = constrain(centerX, 0, SCREEN_WIDTH - 1);
  centerY = constrain(centerY, 0, SCREEN_HEIGHT - 1);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Dist: ");
  display.print(distanceMeters);
  display.print(" m");

  display.setCursor(0, 10);
  display.print("X: ");
  display.print(centerX);
  display.print(" Y: ");
  display.print(centerY);

  display.drawLine(centerX - 10, centerY, centerX + 10, centerY, WHITE);
  display.drawLine(centerX, centerY - 10, centerX, centerY + 10, WHITE);
  display.drawCircle(centerX, centerY, 2, WHITE);

  display.display();
  delay(100);
}
