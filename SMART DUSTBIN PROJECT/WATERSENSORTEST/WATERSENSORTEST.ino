int sensorPin = A0;     // Sensor connected to A0
int sensorValue = 0;

void setup() {
  Serial.begin(9600);   // Start Serial Monitor
}

void loop() {
  sensorValue = analogRead(sensorPin);   // Read sensor value

  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);

  delay(1000);   // Wait 1 second
}