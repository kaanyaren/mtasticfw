#!/usr/bin/env python3
"""
MIDI to Meshtastic Buzzer Converter

This script converts MIDI files to the ToneDuration format used in Meshtastic's buzz.cpp.
It extracts note frequencies and durations from MIDI files and outputs C++ code
that can be used to replace the startup melody in buzz.cpp.

Usage:
    python midi_to_meshtastic.py input.mid [output.cpp]

Requirements:
    pip install mido
"""

import sys
import argparse
import math
from typing import List, Tuple, Optional

try:
    import mido
except ImportError:
    print("Error: mido library not found. Install it with: pip install mido")
    sys.exit(1)

# Note frequency mapping (in Hz) - matches the constants in buzz.cpp
NOTE_FREQUENCIES = {
    # C3 = 131 Hz (matches buzz.cpp)
    60: 131,  # C3
    61: 139,  # C#3
    62: 147,  # D3
    63: 156,  # D#3
    64: 165,  # E3
    65: 175,  # F3
    66: 185,  # F#3
    67: 196,  # G3
    68: 208,  # G#3
    69: 220,  # A3
    70: 233,  # A#3
    71: 247,  # B3
    72: 262,  # C4
    73: 277,  # C#4
    74: 294,  # D4
    75: 311,  # D#4
    76: 330,  # E4
    77: 349,  # F4
    78: 370,  # F#4
    79: 392,  # G4
    80: 415,  # G#4
    81: 440,  # A4
    82: 466,  # A#4
    83: 494,  # B4
    84: 523,  # C5
}

# Duration constants (in milliseconds) - matches buzz.cpp
DURATION_1_8 = 125   # 1/8 note
DURATION_1_4 = 250   # 1/4 note
DURATION_1_2 = 500   # 1/2 note
DURATION_3_4 = 750   # 3/4 note
DURATION_1_1 = 1000  # 1/1 note

class ToneDuration:
    def __init__(self, frequency_hz: int, duration_ms: int):
        self.frequency_hz = frequency_hz
        self.duration_ms = duration_ms

def midi_note_to_frequency(midi_note: int) -> int:
    """Convert MIDI note number to frequency in Hz."""
    if midi_note in NOTE_FREQUENCIES:
        return NOTE_FREQUENCIES[midi_note]
    
    # For notes outside our predefined range, calculate using standard formula
    # A4 (MIDI note 69) = 440 Hz
    return int(440 * (2 ** ((midi_note - 69) / 12)))

def ticks_to_milliseconds(ticks: int, tempo: int, ticks_per_beat: int) -> int:
    """Convert MIDI ticks to milliseconds."""
    # Calculate microseconds per beat
    microseconds_per_beat = tempo
    
    # Convert ticks to milliseconds
    milliseconds = (ticks * microseconds_per_beat) / (ticks_per_beat * 1000)
    return int(milliseconds)

def parse_midi_file(filename: str) -> List[ToneDuration]:
    """Parse MIDI file and extract note events as ToneDuration objects."""
    try:
        mid = mido.MidiFile(filename)
    except Exception as e:
        print(f"Error reading MIDI file: {e}")
        return []
    
    # Track active notes and their start times
    active_notes = {}  # {note: start_time}
    tones = []
    
    # Default tempo (120 BPM = 500000 microseconds per beat)
    tempo = 500000
    ticks_per_beat = mid.ticks_per_beat
    
    for track in mid.tracks:
        current_time = 0
        
        for msg in track:
            current_time += msg.time
            
            if msg.type == 'set_tempo':
                tempo = msg.tempo
            elif msg.type == 'note_on' and msg.velocity > 0:
                # Note starts
                active_notes[msg.note] = current_time
            elif msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0):
                # Note ends
                if msg.note in active_notes:
                    start_time = active_notes[msg.note]
                    duration_ticks = current_time - start_time
                    duration_ms = ticks_to_milliseconds(duration_ticks, tempo, ticks_per_beat)
                    
                    # Only include notes with reasonable duration (10ms to 2000ms)
                    if 10 <= duration_ms <= 2000:
                        frequency = midi_note_to_frequency(msg.note)
                        tones.append(ToneDuration(frequency, duration_ms))
                    
                    del active_notes[msg.note]
    
    return tones

def find_closest_duration(duration_ms: int) -> Tuple[int, str]:
    """Find the closest predefined duration constant."""
    durations = [
        (DURATION_1_8, "DURATION_1_8"),
        (DURATION_1_4, "DURATION_1_4"),
        (DURATION_1_2, "DURATION_1_2"),
        (DURATION_3_4, "DURATION_3_4"),
        (DURATION_1_1, "DURATION_1_1")
    ]
    
    closest_duration = min(durations, key=lambda x: abs(x[0] - duration_ms))
    return closest_duration

def find_closest_note_constant(frequency: int) -> Optional[str]:
    """Find the closest predefined note constant."""
    note_constants = {
        131: "NOTE_C3",
        139: "NOTE_CS3", 
        147: "NOTE_D3",
        156: "NOTE_DS3",
        165: "NOTE_E3",
        175: "NOTE_F3",
        185: "NOTE_FS3",
        196: "NOTE_G3",
        208: "NOTE_GS3",
        220: "NOTE_A3",
        233: "NOTE_AS3",
        247: "NOTE_B3",
        262: "NOTE_C4",
        277: "NOTE_CS4",
        294: "NOTE_D4",
        311: "NOTE_DS4",
        330: "NOTE_E4",
        349: "NOTE_F4",
        370: "NOTE_FS4",
        392: "NOTE_G4",
        415: "NOTE_GS4",
        440: "NOTE_A4",
        466: "NOTE_AS4",
        494: "NOTE_B4",
        523: "NOTE_C5"
    }
    
    return note_constants.get(frequency)

def generate_cpp_code(tones: List[ToneDuration], function_name: str = "playStartMelody") -> str:
    """Generate C++ code for the melody."""
    if not tones:
        return "// No valid tones found in MIDI file\n"
    
    # Limit to reasonable number of notes (max 20 for startup melody)
    if len(tones) > 20:
        tones = tones[:20]
        print(f"Warning: Limited to first 20 notes (original had {len(tones)} notes)")
    
    code_lines = [
        f"void {function_name}()",
        "{",
        "    ToneDuration melody[] = {"
    ]
    
    for i, tone in enumerate(tones):
        # Try to use predefined constants
        note_constant = find_closest_note_constant(tone.frequency_hz)
        duration_constant, duration_name = find_closest_duration(tone.duration_ms)
        
        if note_constant and abs(tone.frequency_hz - tone.frequency_hz) < 5:
            # Use predefined note constant
            note_str = note_constant
        else:
            # Use frequency value
            note_str = str(tone.frequency_hz)
        
        if abs(tone.duration_ms - duration_constant) < 50:
            # Use predefined duration constant
            duration_str = duration_name
        else:
            # Use duration value
            duration_str = str(tone.duration_ms)
        
        comma = "," if i < len(tones) - 1 else ""
        code_lines.append(f"        {{{note_str}, {duration_str}}}{comma}")
    
    code_lines.extend([
        "    };",
        f"    playTones(melody, sizeof(melody) / sizeof(ToneDuration));",
        "}"
    ])
    
    return "\n".join(code_lines)

def main():
    parser = argparse.ArgumentParser(description="Convert MIDI file to Meshtastic buzzer format")
    parser.add_argument("input_midi", help="Input MIDI file")
    parser.add_argument("output_cpp", nargs="?", help="Output C++ file (optional)")
    parser.add_argument("--function", default="playStartMelody", help="Function name (default: playStartMelody)")
    parser.add_argument("--max-notes", type=int, default=20, help="Maximum number of notes (default: 20)")
    
    args = parser.parse_args()
    
    print(f"Converting MIDI file: {args.input_midi}")
    
    # Parse MIDI file
    tones = parse_midi_file(args.input_midi)
    
    if not tones:
        print("No valid notes found in MIDI file")
        return 1
    
    print(f"Found {len(tones)} notes")
    
    # Limit notes if requested
    if len(tones) > args.max_notes:
        tones = tones[:args.max_notes]
        print(f"Limited to first {args.max_notes} notes")
    
    # Generate C++ code
    cpp_code = generate_cpp_code(tones, args.function)
    
    # Output
    if args.output_cpp:
        with open(args.output_cpp, 'w') as f:
            f.write(cpp_code)
        print(f"Generated C++ code written to: {args.output_cpp}")
    else:
        print("\nGenerated C++ code:")
        print("=" * 50)
        print(cpp_code)
        print("=" * 50)
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
