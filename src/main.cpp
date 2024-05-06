/*
 * detect presence,
 * control lightning,
 * publish logs to firebase
 *
 * */
#include "./secrets.h"

#include "Arduino.h"
#include <WiFi.h>
#include <FirebaseESP32.h>

#include <addons/RTDBHelper.h>

/* Wi-Fi credentials */

#ifndef WIFI_SSID
#define WIFI_SSID ${WIFI_SSID_REPO_SECRET}
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ${WIFI_PASSWORD_REPO_SECRET}
#endif


/* RTDB URL and database secret */
#ifndef DATABASE_URL
#define DATABASE_URL ${DATABASE_URL_REPO_SECRET}
#endif

#ifndef DATABASE_SECRET
#define DATABASE_SECRET ${DATABASE_REPO_SECRET}
#endif

/* Firebase data struct object */
FirebaseData fbdo;

/* Firebase data struct for authentication */
FirebaseAuth auth;

/* FirebaseConfig for config data */
FirebaseConfig config;

unsigned long dataMillis = 0;
int count = 0;

void setup() {
    Serial.begin(115200);
    pinMode(BUILTIN_LED,OUTPUT);

    WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
    Serial.print("connecting to Wi-Fi");

    while (WiFi.status() != WL_CONNECTED) {
        /**/
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("connected with IP:");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("\n\n====\nFirebase client: v%s\n====\n", FIREBASE_CLIENT_VERSION);

}

void loop() {
}
