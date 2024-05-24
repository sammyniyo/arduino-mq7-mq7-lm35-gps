#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// Define pins for GPS module
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// Create GPS and software serial objects
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

// Define pins for sensors
const int MQ7Pin = A0; // MQ-7 connected to A0
const int MQ9Pin = A1; // MQ-9 connected to A1
const int LM35Pin = A2; // LM35 connected to A2

// Timing variables for sensor heating cycle
unsigned long previousMillis = 0;
const long interval = 150000; // 2.5 minutes for the full heating cycle of MQ sensors
int heatingPhase = 0; // 0: High voltage, 1: Low voltage

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
  pinMode(MQ7Pin, INPUT);
  pinMode(MQ9Pin, INPUT);
  pinMode(LM35Pin, INPUT); // Ensure the pin mode for LM35 is set to INPUT
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    heatingPhase = 1 - heatingPhase; // Toggle heating phase

    if (heatingPhase == 0) {
      // Set heating elements to high voltage (5V)
    } else {
      // Set heating elements to low voltage (1.4V or 1.5V)
    }
  }

  while (ss.available() > 0) {
    char c = ss.read();
    gps.encode(c);
  }

  // Increase the time to check for GPS data processing to 10 seconds
  if (millis() > 10000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    while (true); // Halt execution if GPS is not detected
  }

  int mq7Value = analogRead(MQ7Pin);
  int mq9Value = analogRead(MQ9Pin);
  float temperature = analogRead(LM35Pin) * (5.0 / 1023.0) * 100.0;

  // Create JSON formatted string for sensor values
  Serial.print("{\"mq7\":");
  Serial.print(mq7Value);
  Serial.print(", \"mq9\":");
  Serial.print(mq9Value);
  Serial.print(", \"temp\":");
  Serial.print(temperature);

  if (gps.location.isValid()) {
    Serial.print(", \"lat\":");
    Serial.print(gps.location.lat(), 6);
    Serial.print(", \"long\":");
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(", \"lat\":null, \"long\":null");
  }

  Serial.println("}");
  delay(1000);
}
