#include <SimpleDHT.h>
#include <ArduinoJson.h> // Add the ArduinoJson library for JSON formatting

// Define analog pins
#define DHT11_PIN A5  // Analog pin for DHT11 sensor
#define IR_SENSOR_PIN A4 // Analog pin for IR sensor
#define LDR_PIN A0     // Analog pin for LDR
#define LED_PIN A1     // Output pin for LED

#define DETECTION_THRESHOLD 5 // Number of consistent detections required

SimpleDHT11 dht11(DHT11_PIN);

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud rate
  pinMode(LED_PIN, OUTPUT); // Initialize LED pin as output
}

void loop() {
  // Read DHT11 sensor
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;

  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); 
    Serial.println(err); 
    delay(500);
    return;
  }

  // Read IR sensor
  int detectionCount = 0;
  for (int i = 0; i < DETECTION_THRESHOLD; i++) {
    int sensorValue = analogRead(IR_SENSOR_PIN); // Read the value from the IR sensor
    if (sensorValue < 512) { // Assuming the sensor outputs a value below 512 when an object is detected
      detectionCount++;
    }
    delay(50); // Delay between readings
  }

  bool irDetected = (detectionCount >= DETECTION_THRESHOLD);

  // Read LDR sensor
  int light = analogRead(LDR_PIN);
  bool lightStatus = (light < 200);

  // Control LED based on LDR reading
  digitalWrite(LED_PIN, lightStatus);

  // Create JSON object
  StaticJsonDocument<256> jsonDoc;
  jsonDoc["temperature"] = (int)temperature;
  jsonDoc["irDetected"] = irDetected;
  jsonDoc["light"] = light;
  jsonDoc["lightStatus"] = lightStatus;

  // Serialize JSON object to string
  String output;
  serializeJson(jsonDoc, output);

  // Print JSON data
  Serial.println(output);

  delay(1000); // Delay to avoid flooding the Serial Monitor
}
