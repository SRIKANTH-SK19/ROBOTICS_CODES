#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

const int sensorPin = A0;

float K = 0.02193;

float filteredV = 0;
float lastV = -1;

void setup() {

  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("WIRELESS CHRG");
}

void loop() {

  long total = 0;

  for(int i=0;i<10;i++) {
    total += analogRead(sensorPin);
    delay(2);
  }

  float adc = total / 10.0;

  float voltage = adc * K;

  // smoother filter
  filteredV = (0.9 * filteredV) + (0.1 * voltage);

  if(filteredV < 0.05)
    filteredV = 0;

  float displayV = round(filteredV * 10.0) / 10.0;

  Serial.println(displayV,1);

  // update LCD only if value changes
  if(displayV != lastV) {

    lcd.setCursor(0,1);
    lcd.print("V: ");
    lcd.print(displayV,1);
    lcd.print("V   ");

    lastV = displayV;
  }

  delay(250);
}