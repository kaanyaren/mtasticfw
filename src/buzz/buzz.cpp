#include "buzz.h"
#include "NodeDB.h"
#include "configuration.h"

#if !defined(ARCH_ESP32) && !defined(ARCH_RP2040) && !defined(ARCH_PORTDUINO)
#include "Tone.h"
#endif

#if !defined(ARCH_PORTDUINO)
extern "C" void delay(uint32_t dwMs);
#endif

struct ToneDuration {
    int frequency_khz;
    int duration_ms;
};

// Some common frequencies.
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_CS4 277

// Some additional notes sometimes referenced
#ifndef NOTE_C4
#define NOTE_C4 261
#endif
#ifndef NOTE_C5
#define NOTE_C5 523
#endif
#ifndef NOTE_B4
#define NOTE_B4 494
#endif
// More notes
#ifndef NOTE_A4
#define NOTE_A4 440
#endif
#ifndef NOTE_AS4
#define NOTE_AS4 466
#endif

// Define a few common notes used by the startup melody if not already present
#ifndef NOTE_D4
#define NOTE_D4 294
#endif
#ifndef NOTE_E4
#define NOTE_E4 329
#endif
#ifndef NOTE_G4
#define NOTE_G4 392
#endif

const int DURATION_1_8 = 125;  // 1/8 note
const int DURATION_1_4 = 250;  // 1/4 note
const int DURATION_1_2 = 500;  // 1/2 note
const int DURATION_3_4 = 750;  // 1/4 note
const int DURATION_1_1 = 1000; // 1/1 note

void playTones(const ToneDuration *tone_durations, int size)
{
    if (config.device.buzzer_mode == meshtastic_Config_DeviceConfig_BuzzerMode_DISABLED ||
        config.device.buzzer_mode == meshtastic_Config_DeviceConfig_BuzzerMode_NOTIFICATIONS_ONLY) {
        // Buzzer is disabled or not set to system tones
        return;
    }
#ifdef PIN_BUZZER
    if (!config.device.buzzer_gpio)
        config.device.buzzer_gpio = PIN_BUZZER;
#endif
    if (config.device.buzzer_gpio) {
        for (int i = 0; i < size; i++) {
            const auto &tone_duration = tone_durations[i];
            tone(config.device.buzzer_gpio, tone_duration.frequency_khz, tone_duration.duration_ms);
            // to distinguish the notes, set a minimum time between them.
            delay(1.3 * tone_duration.duration_ms);
        }
    }
}

void playBeep()
{
    ToneDuration melody[] = {{NOTE_B3, DURATION_1_8}};
    playTones(melody, sizeof(melody) / sizeof(ToneDuration));
}

void playLongBeep()
{
    ToneDuration melody[] = {{NOTE_B3, DURATION_1_1}};
    playTones(melody, sizeof(melody) / sizeof(ToneDuration));
}

void playGPSEnableBeep()
{
    ToneDuration melody[] = {{NOTE_C3, DURATION_1_8}, {NOTE_FS3, DURATION_1_4}, {NOTE_CS4, DURATION_1_4}};
    playTones(melody, sizeof(melody) / sizeof(ToneDuration));
}

void playGPSDisableBeep()
{
    ToneDuration melody[] = {{NOTE_CS4, DURATION_1_8}, {NOTE_FS3, DURATION_1_4}, {NOTE_C3, DURATION_1_4}};
    playTones(melody, sizeof(melody) / sizeof(ToneDuration));
}

void playStartMelody()
{
    // Short bright 5-note startup melody (~0.5s total on typical hardware)
    // Use short durations so the whole tune stays under ~1 second including inter-note gaps.
    const int NOTE_DUR_MS = 80; // each note ~80ms; playTones delays ~1.3x duration between notes

    ToneDuration melody[] = {
        {NOTE_C4, NOTE_DUR_MS},
        {NOTE_D4, NOTE_DUR_MS},
        {NOTE_E4, NOTE_DUR_MS},
        {NOTE_G4, NOTE_DUR_MS},
        {NOTE_C5, NOTE_DUR_MS}
    };

    playTones(melody, sizeof(melody) / sizeof(ToneDuration));
}

void playShutdownMelody()
{
    ToneDuration melody[] = {{NOTE_CS4, DURATION_1_8}, {NOTE_AS3, DURATION_1_8}, {NOTE_FS3, DURATION_1_4}};
    playTones(melody, sizeof(melody) / sizeof(ToneDuration));
}

void playChirp()
{
    // A short, friendly "chirp" sound for key presses
    ToneDuration melody[] = {{NOTE_AS3, 20}}; // Very short AS3 note
    playTones(melody, sizeof(melody) / sizeof(ToneDuration));
}

void playBoop()
{
    // A short, friendly "boop" sound for button presses
    ToneDuration melody[] = {{NOTE_A3, 50}}; // Very short A3 note
    playTones(melody, sizeof(melody) / sizeof(ToneDuration));
}

void playLongPressLeadUp()
{
    // An ascending lead-up sequence for long press - builds anticipation
    ToneDuration melody[] = {
        {NOTE_C3, 100}, // Start low
        {NOTE_E3, 100}, // Step up
        {NOTE_G3, 100}, // Keep climbing
        {NOTE_B3, 150}  // Peak with longer note for emphasis
    };
    playTones(melody, sizeof(melody) / sizeof(ToneDuration));
}

// Static state for progressive lead-up notes
static int leadUpNoteIndex = 0;
static const ToneDuration leadUpNotes[] = {
    {NOTE_C3, 100}, // Start low
    {NOTE_E3, 100}, // Step up
    {NOTE_G3, 100}, // Keep climbing
    {NOTE_B3, 150}  // Peak with longer note for emphasis
};
static const int leadUpNotesCount = sizeof(leadUpNotes) / sizeof(ToneDuration);

bool playNextLeadUpNote()
{
    if (leadUpNoteIndex >= leadUpNotesCount) {
        return false; // All notes have been played
    }

    // Use playTones to handle buzzer logic consistently
    const auto &note = leadUpNotes[leadUpNoteIndex];
    playTones(&note, 1); // Play single note using existing playTones function

    leadUpNoteIndex++;
    return true; // Note was played (playTones handles buzzer availability internally)
}

void resetLeadUpSequence()
{
    leadUpNoteIndex = 0;
}

void playComboTune()
{
    // Quick high-pitched notes with trills
    ToneDuration melody[] = {
        {NOTE_G3, 80},  // Quick chirp
        {NOTE_B3, 60},  // Higher chirp
        {NOTE_CS4, 80}, // Even higher
        {NOTE_G3, 60},  // Quick trill down
        {NOTE_CS4, 60}, // Quick trill up
        {NOTE_B3, 120}  // Ending chirp
    };
    playTones(melody, sizeof(melody) / sizeof(ToneDuration));
}

void playDoubleBeep()
{
    // Two short beeps separated by a short pause
    // playTones expects valid frequency; handle silence by splitting into two calls
    if (config.device.buzzer_gpio) {
        ToneDuration first[] = {{NOTE_B3, 80}};
        ToneDuration second[] = {{NOTE_B3, 80}};
        playTones(first, 1);
        // short pause
        delay(60);
        playTones(second, 1);
    }
}
