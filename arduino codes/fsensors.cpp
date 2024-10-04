#include <SimpleDHT.h>
#include <ArduinoJson.h> // Add the ArduinoJson library for JSON formatting

// Define analog pins
#define DHT11_PIN A5  // Analog pin for DHT11 sensor
#define IR_SENSOR_PIN A4 // Analog pin for IR sensor
#define LDR_PIN A0     // Analog pin for LDR
#define LED_PIN A1     // Output pin for LED

#define DETECTION_THRESHOLD 5 // Number of consistent detections required

// Class for the DHT11 sensor
class DHT11Sensor {
  public:
    DHT11Sensor(int pin) : dht(pin) {}

    void readData(byte &temperature, byte &humidity) {
      int err = SimpleDHTErrSuccess;
      if ((err = dht.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
        Serial.print("Read DHT11 failed, err="); 
        Serial.println(err); 
      }
    }

  private:
    SimpleDHT11 dht;
};

// Class for the IR sensor
class IRSensor {
  public:
    IRSensor(int pin) : sensorPin(pin) {}

    bool detect() {
      int detectionCount = 0;
      for (int i = 0; i < DETECTION_THRESHOLD; i++) {
        int sensorValue = analogRead(sensorPin);
        if (sensorValue < 512) { // Assuming the sensor outputs a value below 512 when an object is detected
          detectionCount++;
        }
        delay(50); // Delay between readings
      }
      return (detectionCount >= DETECTION_THRESHOLD);
    }

  private:
    int sensorPin;
};

// Class for the LDR sensor
class LDRSensor {
  public:
    LDRSensor(int pin, int ledPin) : ldrPin(pin), ledPin(ledPin) {
      pinMode(ledPin, OUTPUT);
    }

    void readData(int &light, bool &lightStatus) {
      light = analogRead(ldrPin);
      lightStatus = (light < 200);
      digitalWrite(ledPin, lightStatus);
    }

  private:
    int ldrPin;
    int ledPin;
};

// Instantiate sensor objects
DHT11Sensor dht11(DHT11_PIN);
IRSensor irSensor(IR_SENSOR_PIN);
LDRSensor ldrSensor(LDR_PIN, LED_PIN);

// Function to handle DHT11 sensor
void handleDHT11() {
  byte temperature = 0;
  byte humidity = 0;
  dht11.readData(temperature, humidity);

  // Add temperature and humidity to JSON object
  StaticJsonDocument<256> jsonDoc;
  jsonDoc["temperature"] = (int)temperature;
  jsonDoc["humidity"] = (int)humidity;
}

// Function to handle IR sensor
void handleIRSensor() {
  bool irDetected = irSensor.detect();
  
  // Add IR detection status to JSON object
  StaticJsonDocument<256> jsonDoc;
  jsonDoc["irDetected"] = irDetected;
}

// Function to handle LDR sensor
void handleLDRSensor() {
  int light = 0;
  bool lightStatus = false;
  ldrSensor.readData(light, lightStatus);
  
  // Add light and light status to JSON object
  StaticJsonDocument<256> jsonDoc;
  jsonDoc["light"] = light;
  jsonDoc["lightStatus"] = lightStatus;
}

// Function to print JSON data
void printJsonData() {
  StaticJsonDocument<256> jsonDoc;
  
  // Handle each sensor and update JSON object
  handleDHT11();
  handleIRSensor();
  handleLDRSensor();

  // Serialize JSON object to string
  String output;
  serializeJson(jsonDoc, output);

  // Print JSON data
  Serial.println(output);
}

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud rate
}

void loop() {
  printJsonData(); // Call the function to print JSON data

  delay(1000); // Delay to avoid flooding the Serial Monitor
}
