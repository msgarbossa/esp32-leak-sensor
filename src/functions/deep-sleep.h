#ifndef DEEP_SLEEP_FUNCTIONS
#define DEEP_SLEEP_FUNCTIONS

#include <Arduino.h>

#include "../config/config.h"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */

extern unsigned long millisOffset;

extern esp_sleep_wakeup_cause_t wakeup_reason;

// Used to calculate amount of awake time to debug and optimize amount of sleep time
unsigned long awakeStartMillis = 0;

// Track how long ESP32 has been running
unsigned long offsetMillis() {
    return millis() + millisOffset;
}

void printUpTime()
{
    unsigned long seconds;
    unsigned int sec, min, hrs;

    seconds = offsetMillis() / 1000;

    sec = seconds % 60;
    seconds /= 60;
    min = seconds % 60;    
    seconds /= 60;
    hrs = seconds % 24;

    Serial.printf("Est. uptime = %02d:%02d:%02d\n", hrs, min, sec);
}

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason(){

  // esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;  // 5
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setup_sleep(int sleep_sec){

  awakeStartMillis = millis();
  Serial.println("[SLEEP] millisOffset: " + String(millisOffset));
  printUpTime();

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every TIME_TO_SLEEP seconds
  */
  esp_sleep_enable_timer_wakeup(sleep_sec * uS_TO_S_FACTOR);
  Serial.println("[SLEEP] Setup ESP32 to sleep for every " + String(sleep_sec) + " Seconds");

  /*
  Next we decide what all peripherals to shut down/keep on
  By default, ESP32 will automatically power down the peripherals
  not needed by the wakeup source, but if you want to be a poweruser
  this is for you. Read in detail at the API docs
  http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
  Left the line commented as an example of how to configure peripherals.
  The line below turns off all RTC peripherals in deep sleep.
  */
  //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  //Serial.println("Configured all RTC Peripherals to be powered down in sleep");

}

void deep_sleep(int sleep_sec){

  /*
  Now that we have setup a wake cause and if needed setup the
  peripherals state in deep sleep, we can now start going to
  deep sleep.
  In the case that no wake up sources were provided but deep
  sleep was started, it will sleep forever unless hardware
  reset occurs.
  */

  unsigned long awakeElapsedMillis = millis() - awakeStartMillis;
  Serial.print("[SLEEP] Going to sleep now (awake " + String(awakeElapsedMillis) + "ms): ");
  delay(1000);
  Serial.flush();

  /*
  Keep track of offsetMilli time while sleeping (assumes sleep until next
  timer wakeup, which isn't true for GPIO wakeups)
  */
  millisOffset = offsetMillis() + sleep_sec * 1000;

  esp_deep_sleep_start();
}

#endif