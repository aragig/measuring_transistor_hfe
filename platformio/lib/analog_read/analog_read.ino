#include "Arduino.h"

#define V_PIN A5

void setup() {
    Serial.begin(115200);
    pinMode(V_PIN, INPUT);
}

void loop() {
    int sensorValue = analogRead(V_PIN);
    Serial.println(sensorValue);
    delay(1000);
}
