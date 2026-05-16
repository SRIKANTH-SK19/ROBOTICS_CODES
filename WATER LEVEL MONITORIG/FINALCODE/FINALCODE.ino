#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define TRIG 4
#define ECHO 5
#define RELAY 6
#define BUZZER 7   // NEW

LiquidCrystal_I2C lcd(0x27, 16, 2);

long duration;
int avgDistance;
int percentage;

// Calibration
const int emptyDistance = 25;
const int fullLevel = 3;

bool buzzerTriggered = false;

// -------- Smooth Reading --------
int readDistance() {
  long sum = 0;

  for (int i = 0; i < 5; i++) {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG, LOW);

    duration = pulseIn(ECHO, HIGH, 30000);

    int d = duration * 0.034 / 2;
    sum += d;

    delay(20);
  }

  return sum / 5;
}

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(RELAY, HIGH);
  digitalWrite(BUZZER, LOW);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Water System");
  lcd.setCursor(0,1);
  lcd.print("Starting...");
  delay(1500);
}

void loop() {

  avgDistance = readDistance();

  if (avgDistance > emptyDistance) avgDistance = emptyDistance;
  if (avgDistance < 0) avgDistance = 0;

  percentage = (emptyDistance - avgDistance) * 100 / (emptyDistance - fullLevel);

  // -------- Pump Control --------
  if (avgDistance <= fullLevel) {
    digitalWrite(RELAY, HIGH); // OFF

    // -------- BUZZER LOGIC --------
    if (!buzzerTriggered) {
      digitalWrite(BUZZER, HIGH); // ON
      delay(3000);                // 3 seconds alert
      digitalWrite(BUZZER, LOW);  // OFF
      buzzerTriggered = true;     // prevent repeat
    }

  } else {
    digitalWrite(RELAY, LOW); // ON
    buzzerTriggered = false;  // reset when water level drops
  }

  // -------- LCD --------
  lcd.setCursor(0,0);
  lcd.print("D:");
  lcd.print(avgDistance);
  lcd.print("cm  ");

  lcd.setCursor(10,0);

  if (avgDistance <= fullLevel) {
    lcd.print("100%");
  } else {
    lcd.print(percentage);
    lcd.print("% ");
  }

  lcd.setCursor(0,1);

  if (avgDistance <= fullLevel) {
    lcd.print("Tank FULL     ");
  } else {
    lcd.print("Filling...    ");
  }

  // -------- Serial --------
  Serial.print("Distance: ");
  Serial.print(avgDistance);
  Serial.print(" cm | Level: ");

  if (avgDistance <= fullLevel)
    Serial.print("100%");
  else
    Serial.print(percentage);

  Serial.print(" | Pump: ");
  Serial.println(avgDistance <= fullLevel ? "OFF" : "ON");

  delay(200);
}