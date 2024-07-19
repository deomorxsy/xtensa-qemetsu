/**
 * Motion detection
 * Detect when the frame changes by a reasonable amount
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = DEBUG"
 * to turn on debug messages
 */
#include "./secrets.c"

#include "Arduino.h"
#include <WiFi.h>
#include <DNSServer.h>
#include <WiFiUdp.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/motion/detection.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>

WiFiClientSecure ssl_client;

//#if defined(ESP32) || defined(ESP8266)
//#include <WiFiClientSecure.h>
//WiFiClientSecure ssl_client;
//#endif

using eloq::camera;
using eloq::motion::detection;


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

DefaultNetwork network;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client, getNetwork(network));
AsyncResult aResult_no_callback;

// RealtimeDatabase
RealtimeDatabase Database;

void asyncCB(AsyncResult &aResult);
void printResult(AsyncResult &aResult);
void printError(int code, const String &msg);


UserAuth user_auth(API_KEY, USER_MAIL, USER_PASSWORD);

FirebaseApp app;

void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___MOTION DETECTION___");

    // camera settings
    // replace with your own model!
    camera.pinout.freenove_s3();
    camera.brownout.disable();
    camera.resolution.vga();
    camera.quality.high();

    // configure motion detection
    // the higher the stride, the faster the detection
    // the higher the stride, the lesser granularity
    detection.stride(1);
    // the higher the threshold, the lesser sensitivity
    // (at pixel level)
    detection.threshold(5);
    // the higher the threshold, the lesser sensitivity
    // (at image level, from 0 to 1)
    detection.ratio(0.2);
    // optionally, you can enable rate limiting (aka debounce)
    // motion won't trigger more often than the specified frequency
    detection.rate.atMostOnceEvery(5).seconds();


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

    // In sync functions, we have to set the operating result for the client that works with the function.
    // This code uses async.
    //client.setAsyncResult(result);

    Serial.println("Realtime Database Setup finished.");

     // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    Serial.println("Camera OK");
    Serial.println("Awaiting for motion...");

}

/**
 *
 */
void loop() {

    // firebase and database loop
    app.loop();
    Database.loop();

    // capture picture
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // run motion detection
    if (!detection.run().isOk()) {
        Serial.println(detection.exception.toString());
        return;
    }

    // on motion, perform action
    if (detection.triggered()) {
        Serial.println("Motion detected!");
        bool motionValue = true;
        Database.push<bool>(aClient, "/sensors/motion", motionValue, asyncCB, "setPirValueTask");

        Serial.print("Motion value: ");
        Serial.println(motionValue);
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

void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}
