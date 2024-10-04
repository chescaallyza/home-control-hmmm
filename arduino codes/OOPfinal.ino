#include <SimpleDHT.h>
#include <ArduinoJson.h> // Add the ArduinoJson library for JSON formatting
#include <Servo.h>

#define DETECTION_THRESHOLD 5 // Number of consistent detections required

class Sensor {
public:
    virtual void read() = 0;
    virtual String toJSON() = 0;
};

class DHT11Sensor : public Sensor {
private:
    SimpleDHT11 dht11;
    byte temperature;
    byte humidity;

public:
    DHT11Sensor(int pin) : dht11(pin), temperature(0), humidity(0) {}

    void read() override {
        int err = SimpleDHTErrSuccess;
        if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
            Serial.print("Read DHT11 failed, err="); 
            Serial.println(err); 
            return;
        }
    }

    String toJSON() override {
        StaticJsonDocument<256> jsonDoc;
        jsonDoc["temperature"] = (int)temperature;
        jsonDoc["humidity"] = (int)humidity;

        String output;
        serializeJson(jsonDoc, output);
        return output;
    }
};

class IRSensor : public Sensor {
private:
    int pin;
    bool motionDetected;
    bool previousDetection;
    unsigned long detectionStartTime;

public:
    IRSensor(int pin) : pin(pin), motionDetected(false), previousDetection(false), detectionStartTime(0) {
        pinMode(pin, INPUT);
    }

    void read() override {
        int detectionCount = 0;
        for (int i = 0; i < DETECTION_THRESHOLD; i++) {
            int sensorValue = digitalRead(pin);
            if (sensorValue == LOW) {
                detectionCount++;
            }
            delay(1);
        }

        bool currentDetection = (detectionCount >= DETECTION_THRESHOLD);
        unsigned long currentMillis = millis();

        if (currentDetection) {
            if (!previousDetection) {
                detectionStartTime = currentMillis;
            }
            if (currentMillis - detectionStartTime <= 5000) {
                motionDetected = true;
            } else {
                motionDetected = false;
            }
        } else {
            detectionStartTime = currentMillis;
        }

        previousDetection = currentDetection;
    }

    String toJSON() override {
        StaticJsonDocument<256> jsonDoc;
        jsonDoc["motionDetected"] = motionDetected;

        String output;
        serializeJson(jsonDoc, output);
        return output;
    }
};

class LDRSensor : public Sensor {
private:
    int pin;
    int light;
    bool lightStatus;

public:
    LDRSensor(int pin) : pin(pin), light(0), lightStatus(false) {
        pinMode(pin, INPUT);
    }

    void read() override {
        light = analogRead(pin);
        lightStatus = (light < 200);
    }

    String toJSON() override {
        StaticJsonDocument<256> jsonDoc;
        jsonDoc["light"] = light;
        jsonDoc["lightStatus"] = lightStatus;

        String output;
        serializeJson(jsonDoc, output);
        return output;
    }

    bool getLightStatus() {
        return lightStatus;
    }
};

class Control {
private:
    int buttonPins[3];
    int ledPins[4];
    int buttonStates[3];
    int lastButtonStates[3];
    Servo servo;
    int controlPin;

public:
    Control(int* btnPins, int* ledPins, int servoPin, int controlPin) : controlPin(controlPin) {
        for (int i = 0; i < 3; i++) {
            buttonPins[i] = btnPins[i];
            pinMode(buttonPins[i], INPUT_PULLUP);
            buttonStates[i] = HIGH;
            lastButtonStates[i] = HIGH;
        }

        for (int i = 0; i < 4; i++) {
            this->ledPins[i] = ledPins[i];
            pinMode(this->ledPins[i], OUTPUT);
            digitalWrite(this->ledPins[i], LOW);
        }

        servo.attach(servoPin);
        pinMode(controlPin, INPUT);
        servo.write(0);
    }

    void manualControl() {
        for (int i = 0; i < 3; i++) {
            buttonStates[i] = digitalRead(buttonPins[i]);

            if (buttonStates[i] != lastButtonStates[i]) {
                if (buttonStates[i] == LOW) {
                    int room = getRoomForButton(i);
                    if (room >= 0) {
                        sendUpdateToWeb(room);
                    }
                }
                delay(50);
            }

            lastButtonStates[i] = buttonStates[i];
        }
    }

    void webControl() {
        int signalState = digitalRead(controlPin);
        if (signalState == HIGH) {
            servo.write(180);
        } else {
            servo.write(90);
        }

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

private:
    int getRoomForButton(int buttonIndex) {
        switch (buttonIndex) {
            case 0: return 1;
            case 1: return 2;
            case 2: return 4;
            default: return -1;
        }
    }

    void sendUpdateToWeb(int room) {
        String output = "{";
        output += "\"room\":\"" + String(room) + "\"";
        output += "}";
        Serial.println(output);
    }
};

DHT11Sensor dht11(A5);
IRSensor irSensor(4);
LDRSensor ldrSensor(A0);
Control control({8, 12, 13}, {5, 6, 10, 11}, 9, 2);

unsigned long previousMillis = 0;
const long interval = 1000;

void setup() {
    Serial.begin(9600);
    pinMode(A1, OUTPUT);
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        dht11.read();
        irSensor.read();
        ldrSensor.read();

        String jsonData = dht11.toJSON() + irSensor.toJSON() + ldrSensor.toJSON();
        Serial.println(jsonData);

        digitalWrite(A1, ldrSensor.getLightStatus());
    }

    control.manualControl();
    control.webControl();
    delay(10);
}
