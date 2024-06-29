#include "./secrets.h"

#include "Arduino.h"
#include "core/AsyncClient/AsyncClient.h"
#include "core/NetConfig.h"
#include "firestore/Documents.h"
#include "firestore/Values.h"
#include <ESP8266WiFi.h>
#include <FirebaseClient.h>
//#include <system_error.h>


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

DefaultNetwork network = 1;

// async
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client, getNetwork(network));

// Firestore
Firestore::Documents Docs;

int counter = 0;
unsigned long dataMillis = 0;
bool taskCompleted = false;

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

    app.loop();

    Docs.loop();

    int pir_signal = digitalRead(PIR_SENSOR_PIN);

    // HIGH
    if(signal == HIGH) {
        digitalWrite(PIN_BUZZER, HIGH);
    }
    // LOW

    if (app.ready() && (millis() - dataMillis > 60000 || dataMillis == 0))
    {
        dataMillis = millis();

        if (!taskCompleted)
        {
            taskCompleted = true;

            Values::MapValue jp("time_zone", Values::IntegerValue(9));
            jp.add("population", Values::IntegerValue(125570000));

            Document<Values::Value> doc("japan", Values::Value(jp));

             Values::MapValue bg("time_zone", Values::IntegerValue(1));
            bg.add("population", Values::IntegerValue(11492641));

            doc.add("Belgium", Values::Value(bg));

            Values::MapValue sg("time_zone", Values::IntegerValue(8));
            sg.add("population", Values::IntegerValue(5703600));

            doc.add("Singapore", Values::Value(sg));

            String documentPath = "info/countries";

            // The value of Values::xxxValue, Values::Value and Document can be printed on Serial.

            Serial.println("Create document... ");

            Docs.createDocument(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, DocumentMask(), doc, asyncCB, "createDocumentTask");
        }

        String documentPath = "info/countries";

        Serial.println("Get a document...");

        Docs.get(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, GetDocumentOptions(DocumentMask("Singapore")), asyncCB, "getTask");
    }

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
