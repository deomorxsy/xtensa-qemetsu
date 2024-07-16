#include "./secrets.c"

#include "Arduino.h"
#include "core/AsyncClient/AsyncClient.h"
#include "core/NetConfig.h"
//#include "firestore/Documents.h"
//#include "firestore/Values.h"
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

#ifndef USER_MAIL
#define USER_MAIL       "USER_MAIL"
#endif

#ifndef USER_PASSWORD
#define USER_PASSWORD   "USER_PASSWORD"
#endif

// RTDB URL and database secret
#ifndef DATABASE_URL
#define DATABASE_URL "URL"
#endif
//#define DATABASE_URL ${DATABASE_URL_REPO_SECRET}

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
AsyncResult aResult_no_callback;

// RealtimeDatabase
RealtimeDatabase Database;

// Firestore
//Firestore::Documents Docs;

bool taskComplete = false;
int counter = 0;
unsigned long dataMillis = 0;

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


    // init Firebase connection
    initializeApp(aClient, app, getAuth(user_auth), asyncCB, "authTask");

    // bind FirebaseApp for authentication handler
    app.getApp<RealtimeDatabase>(Database);
    // unbind with Docs.resetApp();
    //app.getApp<Firestore::Documents>(Docs);

    Database.url(DATABASE_URL);


    Serial.println("Setup finished.");

}

void loop() {

    app.loop();

    //PIR logic
    int pir_signal = digitalRead(PIR_SENSOR_PIN);

    // HIGH
    if (Database.set<bool>(aClient, "/test/bool", true, asyncCB, "setBoolTask")) {
    //if (Firebase.setInt(firebaseData, "/soundSensor", soundValue)) {
    Serial.println("Sound sensor value updated");
    } else {
        Serial.println("Failed to update sound sensor value");
        Serial.println(firebaseData.errorReason());
    }

    if (Firebase.setInt(firebaseData, "/pirSensor", pirValue)) {
        Serial.println("PIR sensor value updated");
    } else {
        Serial.println("Failed to update PIR sensor value");
        Serial.println(firebaseData.errorReason());
    }
    // LOW

    if (app.ready() && (millis() - dataMillis > 60000 || dataMillis == 0))
    {
        dataMillis = millis();

        if (app.ready() && !taskComplete)
        {
            taskComplete = true;


            // Set int
            Database.set<int>(aClient, "/test/int", 12345, asyncCB, "setIntTask");
            // Set bool
            Database.set<bool>(aClient, "/test/bool", true, asyncCB, "setBoolTask");

            // Set string
            Database.set<String>(aClient, "/test/string", "hello", asyncCB, "setStringTask");

            // Set json
            Database.set<object_t>(aClient, "/test/json", object_t("{\"data\":123}"), asyncCB, "setJsonTask");
            //Docs.createDocument(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, DocumentMask(), doc, asyncCB, "createDocumentTask");
            //
            object_t json, obj1, obj2, obj3, obj4;
            JsonWriter writer;

            writer.create(obj1, "int/value", 9999);
            writer.create(obj2, "string/value", string_t("hello"));
            writer.create(obj3, "float/value", number_t(123.456, 2));
            writer.join(obj4, 3 /* no. of object_t (s) to join */, obj1, obj2, obj3);
            writer.create(json, "node/list", obj4);

            // To print object_t
            // Serial.println(json);

            Database.set<object_t>(aClient, "/test/json", json, asyncCB, "setJsonTask");

            object_t arr;
            arr.initArray(); // initialize to be used as array
            writer.join(arr, 4 /* no. of object_t (s) to join */, object_t("[12,34]"), object_t("[56,78]"), object_t(string_t("steve")), object_t(888));

            // Set array
            Database.set<object_t>(aClient, "/test/arr", arr, asyncCB, "setArrayTask");

            // Set float
            Database.set<number_t>(aClient, "/test/float", number_t(123.456, 2), asyncCB, "setFloatTask");

            // Set double
            Database.set<number_t>(aClient, "/test/double", number_t(1234.56789, 4), asyncCB, "setDoubleTask");
        }

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
