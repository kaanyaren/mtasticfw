#include "LedFader.h"
#include "configuration.h"
#include "Arduino.h"
#include "Led.h"

bool LedFader::fadeActive = false;
uint8_t LedFader::brightness = 0;
bool LedFader::increasing = true;
uint32_t LedFader::lastUpdateMs = 0;

void LedFader::begin()
{
#ifdef LED_PIN
    pinMode(LED_PIN, OUTPUT);
    analogWrite(LED_PIN, 0);
#endif
}

void LedFader::update()
{
    if (!fadeActive) {
        return;
    }

    uint32_t now = millis();
    if (now - lastUpdateMs < FADE_INTERVAL_MS) {
        return;
    }
    lastUpdateMs = now;

    // Calculate how many brightness steps to move per interval
    // FADE_DURATION_MS / FADE_INTERVAL_MS gives us total number of updates for full fade
    // BRIGHTNESS_STEPS / (FADE_DURATION_MS / FADE_INTERVAL_MS) gives us increment per update
    float stepsPerUpdate = (float)BRIGHTNESS_STEPS / (FADE_DURATION_MS / FADE_INTERVAL_MS);
    
    if (increasing) {
        brightness += stepsPerUpdate;
        if (brightness >= BRIGHTNESS_STEPS) {
            brightness = BRIGHTNESS_STEPS;
            increasing = false;
        }
    } else {
        if (brightness <= stepsPerUpdate) {
            brightness = 0;
            increasing = true;
        } else {
            brightness -= stepsPerUpdate;
        }
    }

#ifdef LED_PIN
    #if LED_STATE_ON == 0
        analogWrite(LED_PIN, 255 - (brightness * 255 / BRIGHTNESS_STEPS));  // Inverted logic
    #else
        analogWrite(LED_PIN, brightness * 255 / BRIGHTNESS_STEPS);  // Normal logic
    #endif
#endif
}

void LedFader::startFade()
{
    fadeActive = true;
    brightness = 0;
    increasing = true;
    lastUpdateMs = millis();
}

void LedFader::stopFade()
{
    fadeActive = false;
#ifdef LED_PIN
    analogWrite(LED_PIN, LED_STATE_ON ? 0 : 255);  // Turn off LED
#endif
}