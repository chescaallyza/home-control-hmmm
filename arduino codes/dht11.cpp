#include <SimpleDHT.h>

// For DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2

int pinDHT11 = 3;
SimpleDHT11 dht11(pinDHT11);

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Start reading the temperature
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;

  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err); delay(500);
    return;
  }
  
  Serial.print("Temperature: "); Serial.print((int)temperature); Serial.println(" °C");
  
  // Wait a few seconds between measurements
  delay(500);
}