#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

#define BAUDRATE 9600
#define LIMIT 1500

typedef struct Position {
  int x;
  int y;
  int height;
  int width;
  int radius;
  Position() {};
  Position(int x_, int y_) {
    x = x_;
    y = y_;
  }
  Position(int x_, int y_, int radius_) {
    x = x_;
    y = y_;
    radius = radius_;
  }
  Position(int x_, int y_, int height_, int width_) {
    x = x_;
    y = y_;
    height = height_;
    width = width_;
  };
} Position;

int ppm;
Position loadingBarPosition;
Position ppmPosition;
Position circlePosition;


void u8g2_prepare() {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void draw_loading_bar(float percent, Position position) {
  u8g2.drawStr(position.x, position.y - 10, "LoadingBar");
  u8g2.drawBox(position.x + 1, position.y + 1, int(percent * (position.width - 2)), position.height - 2);
  u8g2.drawRFrame(position.x, position.y, position.width, position.height, 3);
}

void draw_loading_bar_with_limit(float value, float limit, Position position) {
  float percentage = value / limit;
  draw_loading_bar(percentage, position);
  u8g2.drawLine(position.x + position.width - 1, position.y - 3, position.x + position.width - 1, position.y + position.height + 3);
}

void print_co2_value(int value, Position position) {
  u8g2.drawStr(position.x, position.y, "Co2 ppm:");
  String value_as_string = String(value);
  u8g2.drawStr(position.x + 50, position.y, value_as_string.c_str());
}

void displayHandler(int ppm, Position loadingBarPosition, Position ppmPosition) {
  draw_loading_bar_with_limit(ppm, LIMIT, loadingBarPosition);
  print_co2_value(ppm, ppmPosition);
}

void drawCircle(Position circlePosition) {
  u8g2.drawDisc(circlePosition.x, circlePosition.y, circlePosition.radius);
}

void setup(void) {
  u8g2.begin();
  u8g2_prepare();
  Serial.begin(BAUDRATE);

  ppm = 100;
  loadingBarPosition = Position(5, 10, 15, 80);
  ppmPosition = Position(5, 30);
  circlePosition = Position(128, 64, 40);
}

void loop(void) {
  u8g2.firstPage();
  do {
    displayHandler(ppm, loadingBarPosition, ppmPosition);

    if (ppm > LIMIT) {
      drawCircle(circlePosition);
    }
  } while ( u8g2.nextPage() );



  if (ppm == 1700) {
    ppm = 0;
  } else {
    ppm = ppm + 50;
  }
  delay(100);

}
