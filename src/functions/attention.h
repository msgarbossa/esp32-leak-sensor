#ifndef ATTENTION_FUNCTIONS
#define ATTENTION_FUNCTIONS

#include <Arduino.h>

#include "../config/config.h"

const int freq1 = 2000;
const int freq2 = 2500;
const int channel = 0;
const int resolution = 8;
bool soundSetup = false;

void setup_sound() {
  pinMode(SOUND_PIN, OUTPUT);
  ledcSetup(channel, freq1, resolution);
  ledcAttachPin(SOUND_PIN, channel);
  soundSetup = true;
}

void play_sound() {
  if (soundSetup == false) {
    setup_sound();
  }
  ledcWriteTone(0,freq1);
  delay(250);
  ledcWriteTone(0,freq2);
  delay(250);
  ledcWrite(channel,0);
}

void blink_now() {
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
}

#endif