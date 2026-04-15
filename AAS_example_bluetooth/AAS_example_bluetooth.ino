#include <SoftwareSerial.h>

unsigned long lastSend = 0;
const unsigned long interval = 1000;

// Arduino RX, TX
SoftwareSerial BTSerial(2, 3);
// BT05 TX -> Arduino pin 2
// BT05 RX -> Arduino pin 3

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  randomSeed(analogRead(A0));
}

void loop() {
  if (millis() - lastSend >= interval) {
    lastSend = millis();

    float doValue   = random(500, 901) / 100.0;
    float phValue   = random(650, 851) / 100.0;
    float tempValue = random(2500, 3201) / 100.0;

    // Shorter BLE-friendly format
    String dataLine = String ("|") + String(doValue, 2) + " mg/L|" +
                      String(phValue, 2) + "|" +
                      String(tempValue, 2)+String(" C");;

    Serial.println(dataLine);

    BTSerial.print(dataLine);
    BTSerial.write((uint8_t)0);   // null terminator
  }
}
