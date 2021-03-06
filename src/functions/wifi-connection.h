#ifndef TASK_WIFI_CONNECTION
#define TASK_WIFI_CONNECTION

#include <Arduino.h>
#include "WiFi.h"
#include "../config/config.h"

void connectWiFi(){

    unsigned long startAttemptTime = millis();
    unsigned short elapsedAttemptTime = 0;

    WiFi.mode(WIFI_STA);
    WiFi.setHostname(DEVICE_NAME);

    unsigned short retryMs = 400;
    while (WiFi.status() != WL_CONNECTED && elapsedAttemptTime < WIFI_TIMEOUT) {
        elapsedAttemptTime = millis() - startAttemptTime;
        Serial.println("[WIFI] connecting (" + String(elapsedAttemptTime) + "ms)");
        WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
        delay(retryMs);
        retryMs = retryMs * 2;
    }

    elapsedAttemptTime = millis() - startAttemptTime;
    // Make sure that we're actually connected
    if(WiFi.status() != WL_CONNECTED){
        Serial.println("[WIFI] FAILED (" + String(elapsedAttemptTime) + "ms): ");
    } else {
        Serial.println("[WIFI] connected (" + String(elapsedAttemptTime) + "ms): ");
        Serial.print(WiFi.localIP());
        Serial.print(F(" "));
        Serial.println(WiFi.macAddress());
    }
}

void disconnectWiFi() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        Serial.println("[WIFI] disconnect/off complete");
    }
}


#endif