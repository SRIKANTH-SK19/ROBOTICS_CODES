#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

const int sensorPin = A0;

// ✅ Final calibrated constant
float K = 0.02586;

float filteredV = 0;

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("WIRELESS EV");
  lcd.setCursor(0,1);
  lcd.print("VOLTAGE");

  delay(1500);
  lcd.clear();
}

void loop() {

  int adc = analogRead(sensorPin);

  // Direct calibrated conversion
  float voltage = adc * K;

  // Smooth filtering
  filteredV = 0.85 * filteredV + 0.15 * voltage;

  if(filteredV < 0.05)
    filteredV = 0;

  Serial.print("ADC=");
  Serial.print(adc);
  Serial.print("  V=");
  Serial.println(filteredV,2);

  lcd.setCursor(0,0);
  lcd.print("WIRELESS POWER");

  lcd.setCursor(0,1);
  lcd.print("V:");
  lcd.print(filteredV,2);
  lcd.print("V   ");

  delay(350);
}