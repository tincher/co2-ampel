#include <U8g2lib.h>
#include <Wire.h>
#include <Arduino.h>
#include <MHZ.h>
#include <SoftwareSerial.h>
#define RX_PIN 5         //MH-Z19 RX-PIN                                         
#define TX_PIN 7         //MH-Z19 TX-PIN  
#define PWMPIN 3
#define MYLEDPIN 9
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

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
MHZ co2(RX_PIN, TX_PIN, PWMPIN, MHZ19B);
SoftwareSerial mySerial(RX_PIN, TX_PIN);

Position loadingBarPosition;
Position ppmPosition;
Position circlePosition;
Position temperaturePosition;

int temperature;
int CO2;
int ppm_uart;
int ppm_pwm;
int preheating = 0;

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
  u8g2.drawStr(5, 50, String(ppm_uart).c_str());
  u8g2.drawStr(50, 50, String(ppm_pwm).c_str());
}

void print_temperature_value(int value, Position position) {
  u8g2.drawStr(position.x, position.y, "Temp:");
  String value_as_string = String(value);
  u8g2.drawStr(position.x + 50, position.y, value_as_string.c_str());
}

void draw_preheating() {
  u8g2.drawStr(50, 50, "Heizt auf ...");
}

void displayHandler(int ppm, int temperature, Position loadingBarPosition, Position ppmPosition, Position tempPosition, int isPreheating) {
  if (isPreheating) {
    draw_preheating();
  } else {
    draw_loading_bar_with_limit(ppm, LIMIT, loadingBarPosition);
    print_co2_value(ppm, ppmPosition);
    print_temperature_value(temperature, tempPosition);
    u8g2.drawStr(5, 50, String(ppm_uart).c_str());
    u8g2.drawStr(50, 50, String(ppm_pwm).c_str());
  }
}

void drawCircle(Position circlePosition) {
  u8g2.drawDisc(circlePosition.x, circlePosition.y, circlePosition.radius);
}

void setup(void) {
  u8g2.begin();
  u8g2_prepare();
  Serial.begin(BAUDRATE);
  pinMode(MYLEDPIN, OUTPUT);
  pinMode(PWMPIN, INPUT);

  delay(100);

  mySerial.begin(BAUDRATE);

  loadingBarPosition = Position(5, 10, 15, 80);
  ppmPosition = Position(5, 30);
  circlePosition = Position(128, 64, 40);
  temperaturePosition = Position(5, 40);

}

void loop() {
  if (!co2.isPreHeating()) {
    ppm_uart = co2.readCO2UART();
    ppm_pwm = co2.readCO2PWM();
    temperature = co2.getLastTemperature();
    CO2 = (ppm_uart + ppm_pwm) / 2;
  }
  int preheating = co2.isPreHeating();
  if (CO2 > LIMIT) {
    digitalWrite(MYLEDPIN, HIGH);
    drawCircle(circlePosition);
  } else {
    digitalWrite(MYLEDPIN, LOW);
  }

  u8g2.firstPage();
  do {
    displayHandler(CO2, temperature, loadingBarPosition, ppmPosition, temperaturePosition, preheating);
  } while ( u8g2.nextPage() );
  delay(2000);
}
