#include <Servo.h>

Servo myservo;

void setup() {
  myservo.attach(5);   // Servo signal wire to pin 5
}

void loop() {

  // Move to 0 degree
  myservo.write(90);
  delay(2000);
}