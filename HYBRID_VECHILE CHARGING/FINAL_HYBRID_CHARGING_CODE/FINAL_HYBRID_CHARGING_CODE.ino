#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL3DvSPHlcj"
#define BLYNK_TEMPLATE_NAME "EV CHARGING"
#define BLYNK_AUTH_TOKEN "1SQSvb1yUTzQpm4ExA3ye_IOu3ufP5pV"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------- WIFI ------------
char ssid[] = "SRI";
char pass[] = "87654321";

// ---------------- LCD -----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- TIMER ---------------
BlynkTimer timer;

// ---------------- VARIABLES -----------
float voltage = 0.0;
float batteryPercent = 0.0;
String status = "";

// --------------------------------------------------
// READ VOLTAGE (FAST + FILTERED)
// --------------------------------------------------
float readVoltage() {
  float sum = 0;

  for (int i = 0; i < 8; i++) {
    sum += analogRead(A0);
    delay(2);
  }

  float avg = sum / 8.0;

  float vout = (avg * 3.3) / 1023.0;

  // Voltage Sensor Ratio
  float vin = vout * 5.0;

  return vin;
}

// --------------------------------------------------
// MAIN FUNCTION
// --------------------------------------------------
void sendData() {

  voltage = readVoltage();

  // ==========================================
  // STATUS CONDITIONS
  // ==========================================

  // NO POWER
  if (voltage < 2.0) {
    status = "No Power";
    batteryPercent = 0;
    voltage = 0;
  }

  // HYBRID MODE (Solar / Charger ON)
  if (voltage >= 4.40) {
    status = "Hybrid";

    batteryPercent = ((voltage - 3.0) / (4.2 - 3.0)) * 100;
  }

  // BATTERY MODE
  if (voltage >= 2.0 && voltage < 4.40) {
    status = "On Battery";

    batteryPercent = ((voltage - 3.0) / (4.2 - 3.0)) * 100;
  }

  batteryPercent = constrain(batteryPercent, 0, 100);

  // ==========================================
  // BLYNK UPDATE
  // ==========================================
  Blynk.virtualWrite(V0, voltage);
  Blynk.virtualWrite(V1, batteryPercent);
  Blynk.virtualWrite(V2, status);

  // ==========================================
  // LCD DISPLAY
  // ==========================================
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltage, 2);
  lcd.print(" ");

  lcd.print("B:");
  lcd.print((int)batteryPercent);
  lcd.print("% ");

  lcd.setCursor(0, 1);

  if (status == "No Power") {
    lcd.print("NO POWER       ");
  }

  if (status == "On Battery") {
    lcd.print("BATTERY MODE   ");
  }

  if (status == "Hybrid") {
    lcd.print("HYBRID MODE    ");
  }

  // ==========================================
  // SERIAL MONITOR
  // ==========================================
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.print("V | ");

  Serial.print("Battery: ");
  Serial.print(batteryPercent);
  Serial.print("% | ");

  Serial.println(status);
}

// --------------------------------------------------
// SETUP
// --------------------------------------------------
void setup() {

  Serial.begin(9600);

  // I2C
  Wire.begin(D2, D1);

  // LCD
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print(" EV CHARGING ");
  lcd.setCursor(0, 1);
  lcd.print(" Initializing");
  delay(2000);

  // Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Fast Update
  timer.setInterval(400L, sendData);
}

// --------------------------------------------------
// LOOP+
// --------------------------------------------------
void loop() {
  Blynk.run();
  timer.run();
}