#include <Servo.h>

Servo esc;

String input = "";
int speedPercent = 0;
int pulse = 1000;
int pwmValue = 0;

unsigned long lastMonitor = 0;
unsigned long lastPlot = 0;

// Slower readable intervals
const unsigned long monitorInterval = 3000;   // every 3 sec
const unsigned long plotInterval    = 300;    // every 300 ms

void setup() {
  Serial.begin(115200);

  esc.attach(9);
  esc.writeMicroseconds(1000);   // Minimum throttle
  delay(5000);                  // ESC arming time

  Serial.println("=================================");
  Serial.println(" BLDC MOTOR CONTROL ");
  Serial.println(" Enter Speed 0 to 100 ");
  Serial.println("=================================");
}

void loop() {

  // -------- Read user input --------
  if (Serial.available()) {

    input = Serial.readStringUntil('\n');
    input.trim();

    speedPercent = input.toInt();

    if (speedPercent >= 0 && speedPercent <= 100) {

      pulse = map(speedPercent, 0, 100, 1000, 2000);
      pwmValue = map(speedPercent, 0, 100, 0, 255);

      esc.writeMicroseconds(pulse);

      Serial.println("---------------------------------");
      Serial.print("Entered Speed : ");
      Serial.print(speedPercent);
      Serial.println("%");

      Serial.print("PWM Value     : ");
      Serial.println(pwmValue);

      Serial.print("Pulse Width   : ");
      Serial.print(pulse);
      Serial.println(" us");
      Serial.println("---------------------------------");
    }

    else {
      Serial.println("Enter value between 0 and 100");
    }
  }

  // -------- Slow readable Serial Monitor output --------
  if (millis() - lastMonitor >= monitorInterval) {

    lastMonitor = millis();

    Serial.print("# Live Speed = ");
    Serial.print(speedPercent);
    Serial.print("%   PWM = ");
    Serial.print(pwmValue);
    Serial.print("   Pulse = ");
    Serial.println(pulse);
  }

  // -------- Serial Plotter numeric values --------
  if (millis() - lastPlot >= plotInterval) {

    lastPlot = millis();

    Serial.print(speedPercent);
    Serial.print(",");
    Serial.print(pwmValue);
    Serial.print(",");
    Serial.println(pulse);
  }
}