#include <Servo.h>

Servo myservo;

// --------------------
// Pins
// --------------------
const byte trigPin = 6;
const byte echoPin = 7;
const byte servoPin = 5;
const byte moisturePin = A0;

// --------------------
// Servo Positions
// 90 = Home
// 135 = Dry
// 45 = Wet
// --------------------
const byte homePos = 90;
const byte dryPos  = 135;
const byte wetPos  = 45;

// --------------------
// Thresholds
// --------------------
const int detectDistance = 15;
const int wetThreshold   = 180;

// --------------------
// Timings
// --------------------
const unsigned long confirmTime  = 500;
const unsigned long cooldownTime = 1200;

// Increased drop time
const unsigned int moveTime   = 900;   // keep side open longer
const unsigned int returnTime = 450;   // stable center

unsigned long detectStart = 0;
unsigned long lastAction = 0;

bool objectDetected = false;

// --------------------
// Distance Read
// --------------------
int getDistance() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long t = pulseIn(echoPin, HIGH, 15000);

  if (t == 0) return 999;

  return t * 0.034 / 2;
}

// --------------------
// Moisture Read
// --------------------
int getMoisture() {

  int a = analogRead(moisturePin);
  int b = analogRead(moisturePin);
  int c = analogRead(moisturePin);

  return (a + b + c) / 3;
}

// --------------------
// Servo Move
// --------------------
void moveServo(byte pos) {

  myservo.attach(servoPin);

  // Move fully to side
  myservo.write(pos);
  delay(moveTime);     // enough time for waste to fall

  // Shake slightly for stuck waste
  myservo.write(pos + 5);
  delay(150);

  myservo.write(pos);
  delay(150);

  // Return Home
  myservo.write(homePos);
  delay(returnTime);

  myservo.detach();
}

// --------------------
// Setup
// --------------------
void setup() {

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  myservo.attach(servoPin);
  myservo.write(homePos);
  delay(400);
  myservo.detach();
}

// --------------------
// Main Loop
// --------------------
void loop() {

  int distance = getDistance();

  if (distance > 2 && distance < detectDistance) {

    if (!objectDetected) {
      objectDetected = true;
      detectStart = millis();
    }

    if ((millis() - detectStart >= confirmTime) &&
        (millis() - lastAction >= cooldownTime)) {

      int moisture = getMoisture();

      if (moisture >= wetThreshold) {
        moveServo(wetPos);
      }
      else {
        moveServo(dryPos);
      }

      lastAction = millis();
      objectDetected = false;
    }
  }
  else {
    objectDetected = false;
  }

  delay(8);
}