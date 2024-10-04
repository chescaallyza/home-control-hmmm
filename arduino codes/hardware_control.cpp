const int buttonPins[] = {8, 12, 13}; // Pins connected to the buttons
const int ledPins[] = {5, 6, 10, 11}; // Pins connected to the LEDs
int buttonStates[3] = {0, 0, 0};      // Array to hold the current state of each button
int lastButtonStates[3] = {0, 0, 0};  // Array to hold the last state of each button

void setup() {
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
}

void loop() {
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

  delay(10); // Small delay for stability
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
