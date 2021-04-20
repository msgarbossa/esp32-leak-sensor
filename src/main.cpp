#include <Arduino.h>
#include <WiFi.h>

#include "config/config.h"
#include "functions/wifi-connection.h"
#include "functions/ota.h"
#include "functions/deep-sleep.h"
#include "functions/mqtt.h"
#include "functions/leak-detect.h"
#include "functions/attention.h"

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int leakStatus;
bool leakChanged;
int8_t wifi_strength;

const unsigned long intervalAlertMillis = 30000;  // How often to alert when leak sensor is wet
RTC_DATA_ATTR unsigned long prevMillisAlert = 0;

const long minIntervalMQTT   = 300000;    // Minimum interval at which to publish metrics (5m = 300,000 milli), higher when sleeping
RTC_DATA_ATTR unsigned long prevMillisMQTT = 0;   // Stores last time sensor was published

// unsigned long int wraps back to 0 after 4,294,967,295 milliseconds or around 50 days
// This variable is used to approximately track time while in deep sleep (instead of calling millis)
RTC_DATA_ATTR unsigned long millisOffset = 0;

esp_sleep_wakeup_cause_t wakeup_reason;

void updateWiFiSignalStrength() {
  if(WiFi.isConnected()) {
    //serial_println(F("[WIFI] Updating signal strength..."));
    wifi_strength = WiFi.RSSI();
    Serial.print("[WIFI] signal strength: ");
    Serial.println(wifi_strength);
  }
}

void callback(){
  //placeholder callback function
  Serial.println("[CALLBACK] Touchpad activated");
}

void setup() {

  Serial.begin(115200);

  // Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  int sleep_sec = TIME_TO_SLEEP_SEC;
  bool forceMqttUpdate = false;

  // Check leak status
  checkLeakStatus();

  // If leak status changes, blink and force MQTT update
  if (leakChanged == true) {
    // play_sound();
    blink_now();
    forceMqttUpdate = true;
  }

  // If wet, do sound alert every intervalAlertMillis
  if (leakStatus == 1) {
    // Check if alert interval has expired
    if (millisOffset - prevMillisAlert >= intervalAlertMillis) {
      prevMillisAlert = millisOffset;
      play_sound();
      blink_now();
    }

    // set sleep sec to use WET_SLEEP_SEC, which is shorter because
    // touch sensor is disabled to avoid constant wakeups when wet.
    sleep_sec = WET_SLEEP_SEC;
  }
  setup_sleep(sleep_sec);


  // publish a new MQTT message every minIntervalMQTT millis
  if ((millisOffset - prevMillisMQTT >= minIntervalMQTT) || (forceMqttUpdate == true)) {
    // Save the last time a new reading was published
    prevMillisMQTT = millisOffset;
    connectWiFi();
    updateWiFiSignalStrength();
    mqttConnect();
    sendUpdateToMQTT();
  }

  // When dry, setup interrupt on Touch Pad 3 (GPIO15)
  // Avoids waiting for timer wakeup interval when leak is present
  if (leakStatus == 0) {
    touchAttachInterrupt(T3, callback, THRESHOLD);
    // Configure Touchpad as wakeup source
    esp_sleep_enable_touchpad_wakeup();
  }

  if (millisOffset > 4000000000) {
    Serial.println("Rebooting due to milliOffset before wraps");
    ESP.restart();
  }

  // Check OTA update on first boot
  if (bootCount == 1) {
    connectWiFi();  // Need to make sure WiFi connected since it's conditional above
    setupOTA();
    delay(2000);
    // takes a few attempts.  worked on count=11 (2s each)
    unsigned short maxRetry = 20;
    unsigned short countRetry = 0;
    while(countRetry <= maxRetry) {
        Serial.println("[OTA] count: " + String(countRetry));
        countRetry++;
        ArduinoOTA.handle();
        delay(2000);
    }
  }

  deep_sleep(sleep_sec);
}

void loop() {
}
