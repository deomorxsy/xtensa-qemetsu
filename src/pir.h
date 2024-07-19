#include <Arduino.h>

#define PIR_SENSOR_PIN D1
#define LED_PIN D8

boolean pirVal = 0;

void setup() {
    Serial.begin(115200);
    pinMode(PIR_SENSOR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    pirVal = digitalRead(PIR_SENSOR_PIN);

    Serial.print("PIR Value: ");
    Serial.println(pirVal);

    if (pirVal == HIGH) {
        digitalWrite(LED_PIN, HIGH);
    } else {
        digitalWrite(LED_PIN, LOW);
    }

    delay(100); // Add a small delay to prevent flooding the serial output
}

