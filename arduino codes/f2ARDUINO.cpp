#include <SimpleDHT.h>
#include <ArduinoJson.h> // Add the ArduinoJson library for JSON formatting
#include <Servo.h>

// Define analog pins
#define DHT11_PIN A5  // Analog pin for DHT11 sensor
#define IR_SENSOR_PIN 4 // Analog pin for IR sensor
#define LDR_PIN A0     // Analog pin for LDR
#define LED_PIN A1     // Output pin for LED

#define DETECTION_THRESHOLD 5 // Number of consistent detections required

SimpleDHT11 dht11(DHT11_PIN);
Servo servo1;

const int buttonPins[] = {8, 12, 13}; // Pins connected to the buttons
const int ledPins[] = {5, 6, 10, 11}; // Pins connected to the LEDs
int buttonStates[3] = {0, 0, 0};      // Array to hold the current state of each button
int lastButtonStates[3] = {0, 0, 0};  // Array to hold the last state of each button

const int controlPin = 2; // Define the pin to read the signal from

unsigned long previousMillis = 0; // Stores the last time the sensors were read
const long interval = 1000; // Interval at which to read sensors

void setup() {
  // Servo setup
  servo1.attach(9); // Attach the servo to pin 9
  pinMode(controlPin, INPUT); // Set the control pin as an input
  pinMode(IR_SENSOR_PIN, INPUT); // Set the IR sensor pin as an input
  servo1.write(0);  // Initialize servo at 0 degrees

  // Setup for buttons
  for (int i = 0; i < 3; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP); // Set the button pins as input with internal pull-up resistors
  }

  // Setup for LEDs
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT); // Set the LED pins as output
    digitalWrite(ledPins[i], LOW); // Initialize LEDs to off
  }

  Serial.begin(9600); // Initialize serial communication
  pinMode(LED_PIN, OUTPUT); // Initialize LED pin as output
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    read_sensors();
  }

  hardware_control();
  webcontrol();
  delay(10); // Small delay for stability
}

void read_sensors() {
  // Read DHT11 sensor
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;

  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); 
    Serial.println(err); 
    return;
  }

  // Read IR sensor for motion detection
  static bool previousDetection = false;
  int detectionCount = 0;
  for (int i = 0; i < DETECTION_THRESHOLD; i++) {
    int sensorValue = digitalRead(IR_SENSOR_PIN); // Read the value from the IR sensor
    if (sensorValue == LOW) { // Assuming the sensor outputs LOW when an object is detected
      detectionCount++;
    }
    delay(10); // Small delay between readings
  }

  bool currentDetection = (detectionCount >= DETECTION_THRESHOLD);
  bool motionDetected = (currentDetection != previousDetection);
  previousDetection = currentDetection;

  // Read LDR sensor
  int light = analogRead(LDR_PIN);
  bool lightStatus = (light < 200);

  // Control LED based on LDR reading
  digitalWrite(LED_PIN, lightStatus);

  // Create JSON object
  StaticJsonDocument<256> jsonDoc;
  jsonDoc["temperature"] = (int)temperature;
  jsonDoc["irDetected"] = motionDetected; // Use motionDetected instead of irDetected
  jsonDoc["light"] = light;
  jsonDoc["lightStatus"] = lightStatus;

  // Serialize JSON object to string
  String output;
  serializeJson(jsonDoc, output);

  // Print JSON data
  Serial.println(output);
}


void hardware_control() {
  // Manual Control
  for (int i = 0; i < 3; i++) {
    buttonStates[i] = digitalRead(buttonPins[i]);
    
    if (buttonStates[i] != lastButtonStates[i]) {
      if (buttonStates[i] == LOW) { // Button pressed
        int room = getRoomForButton(i);
        if (room >= 0) {
          sendUpdateToWeb(room);
        }
      }
      delay(50); // Debounce delay
    }
    
    lastButtonStates[i] = buttonStates[i];
  }
}

void webcontrol() {
  // Servo control
  int signalState = digitalRead(controlPin); // Read the signal state
  if (signalState == HIGH) {
    servo1.write(180); // If the control pin is high, rotate servo to 180 degrees
  } else {
    servo1.write(90); // If the control pin is low, rotate servo to 0 degrees
  }

  // Read from Serial
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    int delimiter = input.indexOf(':');
    if (delimiter > 0) {
      int pin = input.substring(0, delimiter).toInt();
      int state = input.substring(delimiter + 1).toInt();
      digitalWrite(pin, state == 1 ? HIGH : LOW);
      Serial.print("LED on pin ");
      Serial.print(pin);
      Serial.print(" set to ");
      Serial.println(state == 1 ? "ON" : "OFF");
    }
  }
}

int getRoomForButton(int buttonIndex) {
  // Map button index to room
  switch (buttonIndex) {
    case 0: return 1; // Button at pin 8 controls room 1
    case 1: return 2; // Button at pin 12 controls room 2
    case 2: return 4; // Button at pin 13 controls room 4
    default: return -1;
  }
}

void sendUpdateToWeb(int room) {
  // Send room update signal to web
  String output = "{";
  output += "\"room\":\"" + String(room) + "\"";
  output += "}";
  Serial.println(output);
}
