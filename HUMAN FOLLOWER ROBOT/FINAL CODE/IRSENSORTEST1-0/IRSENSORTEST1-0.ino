int irSensorPin = 3;   // IR sensor connected to D2
int irValue = 0;

void setup() {
  pinMode(irSensorPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  irValue = digitalRead(irSensorPin);  // Read IR sensor

  Serial.print("IR Value: ");
  Serial.println(irValue);

  delay(500);  // Small delay for readability
}