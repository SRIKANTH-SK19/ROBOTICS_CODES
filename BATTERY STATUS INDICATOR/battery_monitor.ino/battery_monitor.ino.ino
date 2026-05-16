/*
=========================================================
FINAL SMART BATTERY SYSTEM (ULTIMATE STABLE VERSION)

✔ Voltage stabilized (filter + spike reject)
✔ Current stabilized (no freeze + smart limit)
✔ Realistic time calculation
✔ Fast response (300ms)
✔ Dashboard format unchanged
=========================================================
*/

#include <DHT.h>

// ---------------- DHT11 ----------------
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ---------------- PINS ----------------
#define CURRENT_PIN 34
#define VOLTAGE_PIN 35

// ------------ BATTERY SETTINGS --------
float batteryCapacity_mAh = 2200.0;

// 🔥 CALIBRATED VOLTAGE MULTIPLIER
float voltageMultiplier = 6.38;

// ------------ CURRENT SENSOR ----------
float zero = 0;
float sensitivity = 0.185;
float filteredCurrent = 0;
float alpha = 0.25;   // slightly faster response

// ------------ VOLTAGE FILTER ----------
float filteredVoltage = 0;
float vAlpha = 0.08;

// ------------ TIME STABILITY ----------
float avgCurrent = 0;
float timeAlpha = 0.05;

// =====================================
void setup() {

  Serial.begin(115200);
  delay(2000);

  dht.begin();

  analogReadResolution(12);
  analogSetPinAttenuation(CURRENT_PIN, ADC_11db);
  analogSetPinAttenuation(VOLTAGE_PIN, ADC_11db);

  Serial.println("SYSTEM:STARTING");
  delay(3000);

  calibrateZero();

  Serial.println("SYSTEM:READY\n");
}

// =====================================
void loop() {

  float current = getCurrent();
  float batteryVoltage = getVoltage();

  float temperature = dht.readTemperature();
  if (isnan(temperature)) temperature = 0;

  Serial.println("DATA:START");

  Serial.print("Temperature:");
  Serial.print(temperature);
  Serial.println("C");

  Serial.print("Voltage:");
  Serial.print(batteryVoltage, 2);
  Serial.println("V");

  // -------- NO LOAD ----------
  if (current < 0.10) {

    Serial.println("Status:No Load");

    Serial.print("Current:");
    Serial.print(current, 2);
    Serial.println("A");

    Serial.println("Power:0.00W");
    Serial.println("Time Left:Standby");
  }

  // -------- LOAD ----------
  else {

    Serial.println("Status:Load Connected");

    avgCurrent =
        timeAlpha * current +
        (1 - timeAlpha) * avgCurrent;

    float demoCurrent = avgCurrent;

    // 🔥 prevent unrealistic low current
    if (demoCurrent < 0.30)
      demoCurrent = 0.30;

    float power = batteryVoltage * demoCurrent;

    float hoursLeft =
        (batteryCapacity_mAh / 1000.0) / demoCurrent;

    // 🔥 cap unrealistic time
    if (hoursLeft > 6)
      hoursLeft = 6;

    Serial.print("Current:");
    Serial.print(current, 2);
    Serial.println("A");

    Serial.print("Power:");
    Serial.print(power, 2);
    Serial.println("W");

    int hr = (int)hoursLeft;
    int mn = (hoursLeft - hr) * 60;

    Serial.print("Time Left:");
    Serial.print(hr);
    Serial.print("h ");
    Serial.print(mn);
    Serial.println("min");
  }

  Serial.println("DATA:END\n");

  delay(300);
}

// =====================================
// 🔋 STABLE VOLTAGE FUNCTION
// =====================================
float getVoltage() {

  float adc = readAverageADC(VOLTAGE_PIN, 800);

  if (adc < 5) return 0;

  float sensorVoltage = adc * 3.3 / 4095.0;
  float rawVoltage = sensorVoltage * voltageMultiplier;

  // 🔥 reject spikes
  if (rawVoltage < 2.5 || rawVoltage > 4.5)
    rawVoltage = filteredVoltage;

  // 🔥 smoothing
  filteredVoltage =
      vAlpha * rawVoltage +
      (1 - vAlpha) * filteredVoltage;

  return filteredVoltage;
}

// =====================================
// ⚡ SMART CURRENT FUNCTION (FINAL FIX)
// =====================================
float getCurrent() {

  float adc = readAverageADC(CURRENT_PIN, 300);
  float voltage = adc * 3.3 / 4095.0;

  float rawCurrent = (voltage - zero) / sensitivity;
  rawCurrent = abs(rawCurrent);

  // 🔥 remove small noise
  if (rawCurrent < 0.10)
    rawCurrent = 0;

  // 🔥 smart step limiter (NO FREEZE)
  float maxStep = 0.6;

  if (rawCurrent > filteredCurrent + maxStep)
    rawCurrent = filteredCurrent + maxStep;

  if (rawCurrent < filteredCurrent - maxStep)
    rawCurrent = filteredCurrent - maxStep;

  // 🔥 smoothing
  filteredCurrent =
      alpha * rawCurrent +
      (1 - alpha) * filteredCurrent;

  return filteredCurrent;
}

// =====================================
void calibrateZero() {

  long sum = 0;

  for (int i = 0; i < 1000; i++) {
    sum += analogRead(CURRENT_PIN);
    delayMicroseconds(200);
  }

  float avgADC = sum / 1000.0;

  zero = avgADC * 3.3 / 4095.0;
}

// =====================================
float readAverageADC(int pin, int samples) {

  long total = 0;

  for (int i = 0; i < samples; i++) {
    total += analogRead(pin);
    delayMicroseconds(200);
  }

  return total / (float)samples;
}