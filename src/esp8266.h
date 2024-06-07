#include "./secrets.h"

#include "Arduino.h"
#include "core/AsyncClient/AsyncClient.h"
#include "core/NetConfig.h"
#include "firestore/Documents.h"
#include <ESP8266WiFi.h>
#include <FirebaseClient.h>
#include <system_error.h>


#define PIR_SENSOR_PIN D1

// Wi-Fi credentials
#ifndef WIFI_SSID
#define WIFI_SSID ${WIFI_SSID_REPO_SECRET}
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ${WIFI_PASSWORD_REPO_SECRET}
#endif

// Firebase console > Project Overview > Project settings.
#define API_KEY         "web_API_KEY"

#define USER_MAIL       "USER_MAIL"
#define USER_PASSWORD   "USER_PASSWORD"

// RTDB URL and database secret
#ifndef DATABASE_URL
#define DATABASE_URL ${DATABASE_URL_REPO_SECRET}
#endif

#define FIREBASE_PROJECT_ID "PROJECT_ID"

void asyncCB(AsyncResult &aResult);
void printResult(AsyncResult &aResult);


UserAuth user_auth(API_KEY, USER_MAIL, USER_PASSWORD);

FirebaseApp app;

#if defined(ESP32) || defined(ESP8266)
#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;
#endif

DefaultNetwork network;

// async
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client, getNetwork(network));


void asyncCB(AsyncResult &aResult);
void printResult(AsyncResult &aResult);

// Firestore instantiation
Firestore::Documents Docs;

void setup() {
    Serial.begin(115200);
    pinMode(PIR_SENSOR_PIN, INPUT);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-FI...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.println("Connected to Wi-Fi");

    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

#if defined(ESP32) || defined(ESP8266)
    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif
#endif

    initializeApp(aClient, app, getAuth(user_auth), asyncCB, "authTask");

    app.getApp<Firestore::Documents>(Docs);



    Serial.println("Setup finished.");

}

void loop() {

}

void asyncCB(AsyncResult &aResult)
{
    // WARNING!
    // Do not put your codes inside the callback and printResult.

    printResult(aResult);
}

void printResult(AsyncResult &aResult)
{
    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}
