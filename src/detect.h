#include "Arduino.h"


    // Sensors definition
#define ANALOG_SOUND A0 // brown
#define DIGITAL_SOUND D3 // green, A0

#define PIR_SENSOR_PIN D1
#define LED_PIN D8
//#define BUTTON_PIN D4

boolean pirVal =0;
boolean soundVal=0;
boolean digitalSound=0;
int threshold = 300;



//int ledPin=13;
//int sensorPin=7;
//boolean val =0;

void setup(){
    Serial.begin(115200);
    pinMode(PIR_SENSOR_PIN, INPUT);
    pinMode(DIGITAL_SOUND, INPUT);
    pinMode(ANALOG_SOUND, INPUT);
    pinMode(LED_PIN, OUTPUT);
    //pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop (){
    pirVal =digitalRead(PIR_SENSOR_PIN);
    Serial.println(pirVal);
    soundVal =analogRead(ANALOG_SOUND);
    digitalSound =digitalRead(DIGITAL_SOUND);
    Serial.println(soundVal);

    if (digitalSound > threshold) {
    //if (pirVal == HIGH || soundVal > threshold) {
        digitalWrite(LED_PIN, HIGH);
    }
    else {
        digitalWrite(LED_PIN, LOW);
    }
}
