#pragma once
#include <stdint.h>

/**
 * A class to handle LED fading effects
 */
class LedFader {
public:
    static void begin();
    static void update();
    static bool isActive() { return fadeActive; }
    static void startFade();
    static void stopFade();

private:
    static bool fadeActive;
    static uint8_t brightness;
    static bool increasing;
    static uint32_t lastUpdateMs;
    static constexpr uint32_t FADE_INTERVAL_MS = 10;  // Update every 10ms
    static constexpr uint32_t FADE_DURATION_MS = 3000;  // Complete fade cycle in 3 seconds
    static constexpr uint8_t BRIGHTNESS_STEPS = 100;  // Number of brightness levels
};