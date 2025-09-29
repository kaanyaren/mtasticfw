# MIDI to Meshtastic Buzzer Converter

This tool converts MIDI files to the `ToneDuration` format used in Meshtastic's `buzz.cpp` file.

## Requirements

Install the required Python library:

```bash
pip install mido
```

## Usage

### Basic Usage

```bash
python midi_to_meshtastic.py input.mid
```

This will output the generated C++ code to the console.

### Save to File

```bash
python midi_to_meshtastic.py input.mid output.cpp
```

### Custom Function Name

```bash
python midi_to_meshtastic.py input.mid --function playCustomMelody
```

### Limit Number of Notes

```bash
python midi_to_meshtastic.py input.mid --max-notes 15
```

## How It Works

1. **MIDI Parsing**: The script reads MIDI files and extracts note events (note_on/note_off)
2. **Frequency Mapping**: MIDI note numbers are converted to frequencies in Hz
3. **Duration Calculation**: Note durations are calculated from MIDI timing
4. **Code Generation**: The script generates C++ code using the `ToneDuration` structure

## Supported Features

- **Note Frequencies**: Maps to predefined constants in `buzz.cpp` when possible
- **Duration Constants**: Uses predefined duration constants (DURATION_1_8, DURATION_1_4, etc.)
- **Fallback Values**: Uses raw frequency/duration values when no constant matches
- **Note Limiting**: Limits output to prevent overly long melodies
- **Duration Filtering**: Filters out very short or very long notes

## Example Output

For a simple C-E-G-A melody, the converter generates:

```cpp
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
```

## Integration with buzz.cpp

1. Convert your MIDI file using this tool
2. Copy the generated function code
3. Replace the existing `playStartMelody()` function in `src/buzz/buzz.cpp`
4. Compile and flash your firmware

## Tips for Best Results

- **Keep melodies simple**: Complex MIDI files may not translate well to buzzer format
- **Use monophonic melodies**: Avoid chords and overlapping notes
- **Reasonable tempo**: Very fast or very slow tempos may not work well
- **Short melodies**: Keep startup melodies under 20 notes for best user experience

## Creating Test MIDI Files

You can create simple test MIDI files using:

```bash
python create_test_midi.py
```

This creates a basic C-E-G-A melody for testing the converter.
