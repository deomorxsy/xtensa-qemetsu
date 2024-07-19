#include "./secrets.c"

//type of realtimedatabase return
#include <iostream>
#include <typeinfo>
#include <cxxabi.h>

#include "Arduino.h"
#include "core/AsyncClient/AsyncClient.h"
#include "core/NetConfig.h"
#include "pins_arduino.h"
//#include "firestore/Documents.h"
//#include "firestore/Values.h"
#include <ESP8266WiFi.h>
#include <FirebaseClient.h>
#include <coredecls.h>       // settimeofday_cb()
//#include <system_error.h>

// Sensors definition
    // Sensors definition
#define ANALOG_SOUND A0 // brown
#define DIGITAL_SOUND D3 // green, A0
                         //
#define PIR_SENSOR_PIN D1
#define LED_PIN D8

//#define BUTTON_PIN D4

// Wi-Fi credentials
#ifndef WIFI_SSID
#define WIFI_SSID ${WIFI_SSID_REPO_SECRET}
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ${WIFI_PASSWORD_REPO_SECRET}
#endif

// Firebase console > Project Overview > Project settings.
#ifndef API_KEY
#define API_KEY         "web_API_KEY"
#endif

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
void printError(int code, const String &msg);

void processMessage(char* message);
void sendDataToFireStore(double pressure_val, double temperature_val, double humidity_val, int z_val);

UserAuth user_auth(API_KEY, USER_MAIL, USER_PASSWORD);

// instance of app and firestore docs
FirebaseApp app;
Firestore::Documents Docs;

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

/* Config Variables for specific time zone START */
#define TZ +7     // (utc+) TZ in hours
#define DST_MN 0  // use 60mn for summer time in some countries
#define TZ_MN ((TZ)*60)
#define TZ_SEC ((TZ)*3600)
#define DST_SEC ((DST_MN)*60)

timeval cbtime;  // time set in callback
bool cbtime_set = false;
void time_is_set(void) {
  gettimeofday(&cbtime, NULL);
  cbtime_set = true;
}
/* Config Variables for specific time zone END */

//int counter = 0;
bool taskComplete = false;
unsigned long dataMillis = 0;
const unsigned long interval = 90000; // 1m30s
bool ledState = false;
bool buttonState = false;
bool lastButtonState = false;

int cnt = 0;

//int ledPin=13;
//int sensorPin=7;
boolean pirVal =0;
boolean soundVal=0;
int threshold = 500;

void setup() {
    Serial.begin(115200);
    pinMode(PIR_SENSOR_PIN, INPUT);
    pinMode(SOUND_SENSOR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    /*
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
    */

      /* Calibrate the local time by pulling the UNIX time from the network after connecting to the internet */
    /*
    settimeofday_cb(time_is_set);
    configTime(TZ_SEC, DST_SEC, "pool.ntp.org");

#if defined(ESP32) || defined(ESP8266)
    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif
#endif

    // init Firebase connection
    initializeApp(aClient, app, getAuth(user_auth), asyncCB, "authTask");

    // bind FirebaseApp for authentication handler
    //app.getApp<RealtimeDatabase>(Database);
    // unbind with Docs.resetApp();
    //app.getApp<Firestore::Documents>(Docs);

    Database.url(DATABASE_URL);

    // In sync functions, we have to set the operating result for the client that works with the function.
    // This code uses async.
    //client.setAsyncResult(result);
    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);
    Serial.println("Initializing app...");

    app.getApp<Firestore::Documents>(Docs);

    Serial.println("Setup finished.");*/

}

void loop() {

    //Serial.println("Entering loop...");
    Serial.println("ALO");

    //app.loop();
    //Database.loop();

    pirVal =digitalRead(SOUND_SENSOR_PIN);
    Serial.println(pirVal);
    soundVal =digitalRead(PIR_SENSOR_PIN);
    Serial.println(soundVal);

    if (pirVal == HIGH || soundVal > threshold) {
        digitalWrite(LED_PIN, HIGH);
    }
    else {
        digitalWrite(LED_PIN, LOW);
    }

    /*
    if (app.ready() && !taskComplete)
    {

        Serial.println("Async push!");
        taskComplete = true;
    //unsigned long currentMillis = millis();
    //if (currentMillis - dataMillis >= interval)
    //{
        //dataMillis = currentMillis;

        // We will create the document in the parent path "a0/b?
        // a0 is the collection id, b? is the document id in collection a0.
        String documentPath = "lumenssif/sensors";
        documentPath += cnt;

        // boolean
        Values::BooleanValue bolV(true);

        // Read sensor values
        bool soundValue = digitalRead(SOUND_SENSOR_PIN);
        bool pirValue = digitalRead(PIR_SENSOR_PIN);

        String doc_path = "projects/";
        doc_path += FIREBASE_PROJECT_ID;
        doc_path += "/databases/(default)/documents/coll_id/doc_id"; // coll_id and doc_id are your collection id and document id

        // Send sensor values to Firebase. push to add a line

        Serial.print("Push bool... ");

        // w104 sound sensor
        Database.push<bool>(aClient, "/sensors/sound", soundValue, asyncCB, "setSoundValueTask");
        Serial.print("Sound value: ");
        Serial.println(soundValue);

        // PIR sensor
        Database.push<bool>(aClient, "/sensors/pir", pirValue, asyncCB, "setPirValueTask");
        Serial.print("PIR value: ");
        Serial.println(pirValue);
        /*
        object_t json, sound, pir, rtdb_log;
        JsonWriter writer;

        writer.create(sound, "int/value", 1);
        writer.create(pir, "int/value", 1);
        */
        //writer.join(rtdb_log, 2 /* no. of object_t (s) to join */, sound, pir);
        //writer.create(json, "node/list", rtdb_log);

        /*
         *
         * //send Data to Firestore.
        sendDataToFireStore(pressure_raw, temperature_raw, humidity_raw, z_raw);

        Serial.println("Create document... ");

        Docs.createDocument(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, DocumentMask(), doc, asyncCB, "createDocumentTask");

        // Control LED based on sensor values
        if (!soundValue && !pirValue)
        {
            ledState = false; // Turn off LED
        }
        else
        {
            ledState = true; // Turn on LED
        }
        digitalWrite(LED_PIN, ledState);
    //}
    }

    // Read button state
    buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == LOW && lastButtonState == HIGH)
    {
        ledState = !ledState; // Toggle LED state
        digitalWrite(LED_PIN, ledState);
    }
    lastButtonState = buttonState;
    //if (app.ready() && !taskComplete) {}
    *
    *
    * */
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

void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}



void sendDataToFireStore(double pressure_val, double temperature_val, double humidity_val, int z_val) {
  /*
  This function put sensor data into a desired format.
  Then create a document for Firestore at a specified path.
  */

  //check if FirebaseApp is ready or not
  if (app.ready()) {

    String DOCUMENT_PATH = "sensor_data/d" + String(random(9000000));

    /* format the sensor data */
    Values::DoubleValue pressure(pressure_val);
    Values::IntegerValue date(time(nullptr));
    Values::DoubleValue temperature(temperature_val);
    Values::DoubleValue humidity(humidity_val);
    Values::IntegerValue z(z_val);

    /* Creating a document and push to Firestore*/
    Document<Values::Value> doc("pressure", Values::Value(pressure));
    doc.add("date", Values::Value(date));
    doc.add("temperature", Values::Value(temperature));
    doc.add("humidity", Values::Value(humidity));
    doc.add("z", Values::Value(z));

    Serial.println("Create document... ");
    Docs.createDocument(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), DOCUMENT_PATH, DocumentMask(), doc, aResult_no_callback);
  }
}

std::string demangle(const char* name) {
    int status = -4; // some arbitrary value to eliminate the compiler warning
    char* res = abi::__cxa_demangle(name, NULL, NULL, &status);
    const char* const demangled_name = (status == 0) ? res : name;
    std::string ret_val(demangled_name);
    free(res);
    return ret_val;
}
