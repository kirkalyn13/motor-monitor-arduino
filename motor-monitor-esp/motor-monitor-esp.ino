void setup() {
  Serial.begin(9600); // Initialize Serial communication
  Serial.println("ESP8266 Serial Test Code...");
}

void loop() {
  if ( Serial.available() ) { 
    String data = Serial.readString();
    Serial.print("Received: ");
    Serial.println(data); 
    }

  delay(1000); // Wait for a moment
}
