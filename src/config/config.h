
#ifndef CONFIG
#define CONFIG

#define DEVICE_NAME "leak1"

// How often to wake up from deep sleep
#define TIME_TO_SLEEP_SEC 900

// Sleep less often while wet (leak sensor disabled to avoid constant wakeup)
#define WET_SLEEP_SEC 30

// WiFi
#define WIFI_NETWORK "SSID"
#define WIFI_PASSWORD "PW"
#define WIFI_TIMEOUT 20000 // 20 seconds
#define WIFI_RECOVER_TIME_MS 1000 // (1-20 seconds)

// MQTT
#define MQTT_HOST IPAddress(X, X, X, X)
#define MQTT_PORT 1883
#define MQTT_USER "user"
#define MQTT_PASSWORD "password"
#define MQTT_CONNECT_DELAY 200
#define MQTT_CONNECT_TIMEOUT 10000 // 10 seconds

// Pins
#define LED_PIN 2 // for activity blink
#define SOUND_PIN 32 // for alerts
#define LEAK_TOUCH_PIN 15

// sensitivity for the tocuh sensor
// greater number for more sensitivity
// test final setup, but somewhere between 5 and 40 seems to work
#define THRESHOLD 20

#endif
