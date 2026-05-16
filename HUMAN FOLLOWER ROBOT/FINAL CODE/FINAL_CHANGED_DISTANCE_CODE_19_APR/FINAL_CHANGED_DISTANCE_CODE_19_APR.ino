#define IR_LEFT 2
#define IR_RIGHT 3

#define echoPin 4
#define trigPin 7

#define ENA 5
#define ENB 6

#define in1 A0
#define in2 A1
#define in3 A2
#define in4 A3

// ---------------- SPEED ----------------
const int baseSpeed = 190;
const int turnSpeed = 140;

// ---------------- DISTANCE RANGE ----------------
// Follow only when >30 cm and <60 cm
const int STOP_DIST  = 30;   // <=30 immediate stop
const int FOLLOW_MAX = 60;   // >=60 stop

// ---------------- TIMING ----------------
const unsigned long pingInterval = 15;   // faster response
const unsigned long turnTime     = 500;  // IR active time

// ---------------- VARIABLES ----------------
unsigned long lastPing = 0;
unsigned long turnStart = 0;

int distance = 100;   // default no target
bool turning = false;
int turnDir = 0;      // 1=right, -1=left

// ---------------- SETUP ----------------
void setup() {

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  stopMotors();
}

// ---------------- MAIN LOOP ----------------
void loop() {

  unsigned long now = millis();

  // FAST ultrasonic refresh
  if (now - lastPing >= pingInterval) {
    distance = readDistance();
    lastPing = now;
  }

  // -------- EMERGENCY STOP --------
  if (distance <= STOP_DIST) {
    stopMotors();
    turning = false;
    return;
  }

  // -------- HANDLE TURNING --------
  if (turning) {
    if (now - turnStart < turnTime) {
      if (turnDir == 1) slightRight();
      else slightLeft();
      return;
    } else {
      turning = false;
    }
  }

  // -------- IR PRIORITY --------
  int leftIR  = digitalRead(IR_LEFT);
  int rightIR = digitalRead(IR_RIGHT);

  if (rightIR == 0) {
    delay(4); // quick debounce
    if (digitalRead(IR_RIGHT) == 0) {
      turning = true;
      turnDir = 1;
      turnStart = now;
      slightRight();
      return;
    }
  }

  if (leftIR == 0) {
    delay(4);
    if (digitalRead(IR_LEFT) == 0) {
      turning = true;
      turnDir = -1;
      turnStart = now;
      slightLeft();
      return;
    }
  }

  // -------- FOLLOW LOGIC --------
  if (distance > STOP_DIST && distance < FOLLOW_MAX) {
    moveForward();
  } else {
    stopMotors();
  }
}

// ---------------- ULTRASONIC ----------------
int readDistance() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long t = pulseIn(echoPin, HIGH, 7000);

  // no echo = no object
  if (t == 0) return 200;

  int d = t / 58;

  if (d < 2 || d > 250) return 200;

  return d;
}

// ---------------- MOVEMENT ----------------
void moveForward() {

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
}

void slightLeft() {

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  analogWrite(ENA, turnSpeed);
  analogWrite(ENB, baseSpeed);
}

void slightRight() {

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, turnSpeed);
}

// ---------------- FAST STOP ----------------
void stopMotors() {

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}