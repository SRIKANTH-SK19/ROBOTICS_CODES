#define TRIG 3
#define ECHO 2

int duration;
int distance;

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  
  Serial.begin(9600);
  Serial.println("Ultrasonic Sensor Test");
}

void loop() {

  // Send trigger pulse
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  // Read echo pulse
  duration = pulseIn(ECHO, HIGH);

  // Convert to distance (cm)
  distance = duration * 0.034 / 2;

  // Print result
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500);
}