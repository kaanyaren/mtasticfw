void playStartMelody()
{
    ToneDuration melody[] = {
        {NOTE_C3, DURATION_1_4},
        {NOTE_E3, DURATION_1_4},
        {NOTE_G3, DURATION_1_4},
        {NOTE_A3, DURATION_1_2}
    };
    playTones(melody, sizeof(melody) / sizeof(ToneDuration));
}