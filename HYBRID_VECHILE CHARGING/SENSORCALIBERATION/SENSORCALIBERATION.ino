#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// I2C LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Sensor pin
const int sensorPin = A0;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("CURRENT SENSOR");
  lcd.setCursor(0,1);
  lcd.print("CALIBRATING...");
  
  delay(2000);
}

void loop() {

  long total = 0;

  // Take 500 samples for stable zero reading
  for (int i = 0; i < 500; i++) {
    total += analogRead(sensorPin);
    delay(5);
  }

  float avgADC = total / 500.0;

  float voltage = (avgADC / 1023.0) * 5000.0;   // mV

  Serial.print("ADC = ");
  Serial.print(avgADC,2);

  Serial.print("   Zero Voltage(mV) = ");
  Serial.println(voltage,2);

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("ADC=");
  lcd.print(avgADC,1);

  lcd.setCursor(0,1);
  lcd.print("Zero=");
  lcd.print(voltage,0);
  lcd.print("mV");

  delay(1000);
}