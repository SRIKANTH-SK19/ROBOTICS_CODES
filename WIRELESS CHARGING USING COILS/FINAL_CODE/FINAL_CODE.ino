#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

const int sensorPin = A0;

// Calibrated constant
float K = 0.0252;

float filteredV = 0;
float lastV = -1;

void setup() {

  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("WIRELESS CHRG");

  lcd.setCursor(0,1);
  lcd.print("INITIALIZING");

  delay(1500);

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("WIRELESS CHRG");
}

void loop() {

  long total = 0;

  // Multi-sampling for stable readings
  for(int i=0; i<10; i++) {

    total += analogRead(sensorPin);
    delay(2);
  }

  float adc = total / 10.0;

  // Voltage calculation
  float voltage = adc * K;

  // Smooth filtering
  filteredV = (0.9 * filteredV) + (0.1 * voltage);

  // Remove tiny noise
  if(filteredV < 0.05)
    filteredV = 0;

  // Limit maximum display
  if(filteredV > 5.5)
    filteredV = 5.5;

  // One digit after decimal
  float displayV = round(filteredV * 10.0) / 10.0;

  // Serial Monitor
  Serial.print("Voltage: ");
  Serial.println(displayV,1);

  // Update LCD only if value changes
  if(displayV != lastV) {

    lcd.setCursor(0,1);

    lcd.print("V: ");
    lcd.print(displayV,1);
    lcd.print("V   ");

    lastV = displayV;
  }

  delay(250);
}