int trigPin = 6;
int echoPin = 7;

long duration;
float distance;
float lastDistance = 0;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}

void loop() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 12000);

  if (duration == 0) return;

  distance = duration * 0.034 / 2;

  // Ignore crazy jump
  if (abs(distance - lastDistance) > 25 && lastDistance != 0) {
    distance = lastDistance;
  }

  lastDistance = distance;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(1000);
}