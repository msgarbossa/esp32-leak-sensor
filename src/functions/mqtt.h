#ifndef TASK_MQTT
#define TASK_MQTT

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "../config/config.h"

// WiFiClientSecure MQTT_net;
WiFiClient MQTT_net;
PubSubClient MQTT_client(MQTT_net);

extern int leakStatus;
extern int8_t wifi_strength;

void mqttConnect() {

    if(!WiFi.isConnected()){
        Serial.println("[MQTT] Failed.  WiFi not connected.");
        return;
    }

    Serial.println(F("[MQTT] Connecting to HA..."));
    MQTT_client.setServer(MQTT_HOST, MQTT_PORT);

    long startAttemptTime = millis();

    while (!MQTT_client.connect(DEVICE_NAME, MQTT_USER, MQTT_PASSWORD) &&
        millis() - startAttemptTime < MQTT_CONNECT_TIMEOUT) {
        delay(MQTT_CONNECT_DELAY);
    }

    if(!MQTT_client.connected()){
        Serial.println("[MQTT] Failed.  Connection attemp timeout.");
    }

    unsigned long MqttConnectTime = millis() - startAttemptTime;
    Serial.println("[MQTT] Connected (" + String(MqttConnectTime) + "ms)!");
}


void sendUpdateToMQTT() {

    if(!MQTT_client.connected()){
        Serial.println(F("[MQTT] Connection failed for temperature."));
        return;
    }

    if(!MQTT_client.connected()){
        Serial.println(F("[MQTT] Can't send to HA without MQTT. Abort."));
        return;
    }

    // variable for MQTT payload
    char msg[50];
    sprintf(msg, "{\"w\":\"%d\",\"s\":\"%d\"}", leakStatus, wifi_strength);

    Serial.print(F("[MQTT] HA publish: "));
    Serial.println(msg);

    MQTT_client.publish("home/" DEVICE_NAME "/metrics", msg);
    Serial.printf("[MQTT] published to home/%s/metrics\n", DEVICE_NAME);
}

#endif
