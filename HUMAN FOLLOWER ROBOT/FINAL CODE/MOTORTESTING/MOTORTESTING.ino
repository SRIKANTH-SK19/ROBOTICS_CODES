int ENA = 5;
int ENB = 6;

int in1 = A0;
int in2 = A1;
int in3 = A2;
int in4 = A3;

int speedVal = 80; // 0–255

char command;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  Serial.begin(9600);
  Serial.println("Enter F/B/L/R/S to control motor");
}

void loop() {
  if (Serial.available()) {
    command = Serial.read();

    switch (command) {

      case 'F': // Forward
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
        analogWrite(ENA, speedVal);
        analogWrite(ENB, speedVal);
        Serial.println("Moving Forward");
        break;

      case 'B': // Backward
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
        analogWrite(ENA, speedVal);
        analogWrite(ENB, speedVal);
        Serial.println("Moving Backward");
        break;

      case 'L': // Left
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
        analogWrite(ENA, speedVal);
        analogWrite(ENB, speedVal);
        Serial.println("Turning Left");
        break;

      case 'R': // Right
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
        analogWrite(ENA, speedVal);
        analogWrite(ENB, speedVal);
        Serial.println("Turning Right");
        break;

      case 'S': // Stop
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        analogWrite(ENA, 0);
        analogWrite(ENB, 0);
        Serial.println("Stopped");
        break;

      default:
        Serial.println("Invalid Command");
    }
  }
}