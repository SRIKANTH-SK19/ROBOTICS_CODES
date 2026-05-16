#define BLYNK_TEMPLATE_ID "TMPL3iCUhgQ7a"
#define BLYNK_TEMPLATE_NAME "CANAL CLEANER ROBOT"
#define BLYNK_AUTH_TOKEN "324q0N3v889PiwHUuR-2GAJXrbXkr8Tl"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// ----------------------
// WiFi Credentials
// ----------------------
char ssid[] = "IPHONE";
char pass[] = "12345678";

// ----------------------
// L298N Motor Driver Pinscv
// ----------------------

// Boat Propeller Motor
#define ENA D3
#define IN1 D1
#define IN2 D2

// Conveyor Belt Motor
#define ENB D4
#define IN3 D5
#define IN4 D6

// ----------------------
// Ultrasonic Sensor Pins
// ----------------------
#define TRIG D7
#define ECHO D8

// ----------------------
BlynkTimer timer;

// Speed Variables
int boatSpeed = 800;
int conveyorSpeed = 400;

// Motor States
bool boatState = false;
bool conveyorState = false;

// ----------------------
// Ultrasonic Function
// ----------------------
long getDistance()
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);

  long distance = duration * 0.034 / 2;

  // Avoid false values
  if(distance <= 0 || distance > 400)
  {
    distance = 400;
  }

  return distance;
}

// ----------------------
// Send Distance to Blynk
// ----------------------
void sendDistance()
{
  long distance = getDistance();

  Blynk.virtualWrite(V2, distance);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

// ----------------------
// Boat Motor Control
// ----------------------
void runBoatMotor()
{
  if(boatState)
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    analogWrite(ENA, boatSpeed);
  }
  else
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    analogWrite(ENA, 0);
  }
}

// ----------------------
// Conveyor Motor Control
// ----------------------
void runConveyorMotor()
{
  if(conveyorState)
  {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    analogWrite(ENB, conveyorSpeed);
  }
  else
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    analogWrite(ENB, 0);
  }
}

// ----------------------
// Boat Switch
// ----------------------
BLYNK_WRITE(V0)
{
  boatState = param.asInt();

  runBoatMotor();

  Serial.print("Boat Motor: ");

  if(boatState)
    Serial.println("ON");
  else
    Serial.println("OFF");
}

// ----------------------
// Conveyor Switch
// ----------------------
BLYNK_WRITE(V1)
{
  conveyorState = param.asInt();

  runConveyorMotor();

  Serial.print("Conveyor: ");

  if(conveyorState)
    Serial.println("ON");
  else
    Serial.println("OFF");
}

// ----------------------
// Boat Speed Slider
// ----------------------
BLYNK_WRITE(V3)
{
  boatSpeed = param.asInt();

  if(boatState)
  {
    analogWrite(ENA, boatSpeed);
  }

  Serial.print("Boat Speed: ");
  Serial.println(boatSpeed);
}

// ----------------------
// Conveyor Speed Slider
// ----------------------
BLYNK_WRITE(V4)
{
  conveyorSpeed = param.asInt();

  if(conveyorState)
  {
    analogWrite(ENB, conveyorSpeed);
  }

  Serial.print("Conveyor Speed: ");
  Serial.println(conveyorSpeed);
}

// ----------------------
// Blynk Connected
// ----------------------
BLYNK_CONNECTED()
{
  Serial.println("Blynk Connected");

  Blynk.syncAll();
}

// ----------------------
// Setup
// ----------------------
void setup()
{
  Serial.begin(115200);

  // Motor Pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Ultrasonic
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // Better PWM Stability
  analogWriteRange(1023);
  analogWriteFreq(1000);

  // Motors OFF Initially
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  Serial.println();
  Serial.println("Connecting to WiFi...");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Fast Real-Time Updates
  timer.setInterval(300L, sendDistance);

  Serial.println("System Ready");
}

// ----------------------
// Main Loop
// ----------------------
void loop()
{
  Blynk.run();
  timer.run();
}