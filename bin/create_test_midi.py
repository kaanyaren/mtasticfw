#!/usr/bin/env python3
"""
Create a simple test MIDI file for testing the converter.
"""

try:
    import mido
except ImportError:
    print("Error: mido library not found. Install it with: pip install mido")
    exit(1)

def create_test_midi():
    """Create a simple test MIDI file with a basic melody."""
    # Create a new MIDI file
    mid = mido.MidiFile()
    track = mido.MidiTrack()
    mid.tracks.append(track)
    
    # Set tempo (120 BPM)
    track.append(mido.MetaMessage('set_tempo', tempo=500000, time=0))
    
    # Simple melody: C-E-G-A (ascending arpeggio)
    notes = [60, 64, 67, 69]  # C3, E3, G3, A3
    durations = [250, 250, 250, 500]  # Quarter, quarter, quarter, half note
    
    time = 0
    for note, duration in zip(notes, durations):
        # Note on
        track.append(mido.Message('note_on', channel=0, note=note, velocity=64, time=time))
        time = 0  # Reset time for note_off
        
        # Note off
        track.append(mido.Message('note_off', channel=0, note=note, velocity=64, time=duration))
    
    # Save the file
    mid.save('test_melody.mid')
    print("Created test_melody.mid")

if __name__ == "__main__":
    create_test_midi()
