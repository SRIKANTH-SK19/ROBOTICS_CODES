#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

const int sensorPin = A0;

// 🔧 Final calibrated constant
float K = 0.02193;

float filteredV = 0;
float lastDisplayV = -1;

// Custom icons
byte battery[8] = {
  0b01110,0b10001,0b10001,0b11111,
  0b11111,0b10001,0b10001,0b11111
};

byte lightning[8] = {
  0b00100,0b01110,0b00100,0b01110,
  0b00100,0b01110,0b00100,0b00000
};

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  lcd.createChar(0, battery);
  lcd.createChar(1, lightning);

  lcd.setCursor(0,0);
  lcd.print("WIRELESS EV");
  lcd.setCursor(0,1);
  lcd.print("SYSTEM READY");

  delay(1500);
  lcd.clear();
}

void loop() {

  // -------- MULTI SAMPLE --------
  long total = 0;
  for(int i=0;i<5;i++){
    total += analogRead(sensorPin);
    delay(2);
  }

  float adc = total / 5.0;

  float voltage = adc * K;

  // -------- SMOOTH FILTER --------
  filteredV = 0.7 * filteredV + 0.3 * voltage;

  // -------- CLAMP --------
  if(filteredV > 5.5) filteredV = 5.5;
  if(filteredV < 0.05) filteredV = 0;

  // -------- ROUND TO 1 DECIMAL --------
  float displayV = round(filteredV * 10.0) / 10.0;

  // -------- CHARGING LOGIC --------
  bool charging = (displayV >= 2.0);

  // -------- SERIAL DEBUG --------
  Serial.print("ADC=");
  Serial.print(adc);
  Serial.print("  V=");
  Serial.println(displayV,1);

  // -------- LCD LINE 1 --------
  lcd.setCursor(0,0);

  if(charging){
    lcd.print("EV ON CHARGE ");
    lcd.write(byte(1));
  } else {
    lcd.print("EV ON BATTERY");
    lcd.write(byte(0));
  }

  // -------- LCD LINE 2 (NO FLICKER) --------
  if(displayV != lastDisplayV){
    lcd.setCursor(0,1);
    lcd.print("V:");
    lcd.print(displayV,1);
    lcd.print("V   ");
    lastDisplayV = displayV;
  }

  delay(80);
}