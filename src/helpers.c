#include "midifile.h"
#include "helpers.h"
#include "hal/emidi_hal.h"

const char* eMidi_errorToStr(Error error) {
  switch(error) {
    case EMIDI_OK:                       return "Ok";
    case EMIDI_OK_END_OF_FILE:           return "Ok, end of file";
    case EMIDI_INVALID_HANDLE:           return "Invalid Handle";
    case EMIDI_CANNOT_OPEN_FILE:         return "Cannot open file";
    case EMIDI_INVALID_MIDI_FILE:        return "Invalid MIDI file";
    case EMIDI_UNEXPECTED_END_OF_FILE:   return "Unexpected end of File";
    case EMIDI_SYNTAX_ERROR:             return "Syntax Error";
    case EMIDI_FUNCTION_NOT_IMPLEMENTED: return "Function not implemented";
    case EMIDI_FORMAT_1_NOT_SUPPORTED:   return "Format 1 not supported";
    case EMIDI_FORMAT_2_NOT_SUPPORTED:   return "Format 2 not supported";

    default:                             return "Unknown";
  }
}

const char* eMidi_eventToStr(uint8_t eventCode) {
  uint8_t channelMessage = eventCode & 0xF0;
  uint8_t systemMessage  = eventCode;

  switch(channelMessage) {
    case MIDI_EVENT_NOTE_OFF:                return "Note-Off";
    case MIDI_EVENT_NOTE_ON:                 return "Note-On";
    case MIDI_EVENT_POLY_KEY_PRESSURE:       return "Poly Key Pressure";
    case MIDI_EVENT_CONTROL_CHANGE:          return "Control Change";
    case MIDI_EVENT_PROGRAM_CHANGE:          return "Program Change";
    case MIDI_EVENT_CHANNEL_PRESSURE:        return "Channel Pressure";
    case MIDI_EVENT_PITCH_BEND:              return "Pitch Bend";
  }

  switch(systemMessage) {
    case MIDI_EVENT_META:                    return "Meta Event";
    case MIDI_EVENT_SYSTEM_EXCLUSIVE:        return "System Exclusive";
    case MIDI_EVENT_TIME_CODE_QUARTER_FRAME: return "MIDI Time Code Quarter Frame";
    case MIDI_EVENT_SONG_POSITION_POINTER:   return "Song Position Pointer";
    case MIDI_EVENT_SONG_SELECT:             return "Song Select";
    case MIDI_EVENT_TUNE_REQUEST:            return "Tune Request";
    case MIDI_EVENT_END_OF_EXCLUSIVE:        return "EOX (End of Exclusive)";
    case MIDI_EVENT_TIMING_CLOCK:            return "Real Time Timing Clock";
    case MIDI_EVENT_START:                   return "Real Time Start";
    case MIDI_EVENT_CONTINUE:                return "Real Time Continue";
    case MIDI_EVENT_STOP:                    return "Real Time Stop";
    case MIDI_EVENT_ACTIVE_SENSING:          return "Real Time Active Sensing";

    // TODO: following event collides with meta event!?
    // case MIDI_EVENT_SYSTEM_RESET:            "Real Time System Reset";

    default:                                 return "Unknown";
  }
}

const char* eMidi_metaEventToStr(int metaEventId) {
  switch(metaEventId) {
    case MIDI_META_SEQUENCE_NUMBER:     return "Sequence Number";
    case MIDI_META_TEXT_EVENT:          return "Text Event";
    case MIDI_META_COPYRIGHT_NOTICE:    return "Copyright Notice";
    case MIDI_META_SEQUENCE_TRACK_NAME: return "Sequence Track Name";
    case MIDI_META_INSTRUMENT_NAME:     return "Instrument Name";
    case MIDI_META_LYRIC:               return "Lyric";
    case MIDI_META_MARKER:              return "Marker";
    case MIDI_META_CUE_POINT:           return "Cue Point";
    case MIDI_META_MIDI_CHANNEL_PREFIX: return "MIDI Channel Prefix";
    case MIDI_META_MIDI_PORT:           return "MIDI Port";
    case MIDI_END_OF_TRACK:             return "End of Track";
    case MIDI_SET_TEMPO:                return "Set Tempo";
    case MIDI_SMPTE_OFFSET:             return "SMPTE Offset";
    case MIDI_TIME_SIGNATURE:           return "Time Signature";
    case MIDI_KEY_SIGNATURE:            return "Key Signature";
    case MIDI_SEQUENCER_SPECIFIC:       return "Sequencer Specific";

    default:                            return "Unknown";
  }
}

const char* eMidi_numberToNote(uint8_t midiNote) {
  switch(midiNote) {
    case 0:  return "C-1";
    case 1:  return "C#-1";
    case 2:  return "D-1";
    case 3:  return "D#-1";
    case 4:  return "E-1";
    case 5:  return "F-1";
    case 6:  return "F#-1";
    case 7:  return "G-1";
    case 8:  return "G#-1";
    case 9:  return "A-1";
    case 10: return "A#-1";
    case 11: return "B-1";
    case 12: return "C0";
    case 13: return "C#0";
    case 14: return "D0";
    case 15: return "D#0";
    case 16: return "E0";
    case 17: return "F0";
    case 18: return "F#0";
    case 19: return "G0";
    case 20: return "G#0";
    case 21: return "A0";
    case 22: return "A#0";
    case 23: return "B0";
    case 24: return "C1";
    case 25: return "C#1";
    case 26: return "D1";
    case 27: return "D#1";
    case 28: return "E1";
    case 29: return "F1";
    case 30: return "F#1";
    case 31: return "G1";
    case 32: return "G#1";
    case 33: return "A1";
    case 34: return "A#1";
    case 35: return "B1";
    case 36: return "C2";
    case 37: return "C#2";
    case 38: return "D2";
    case 39: return "D#2";
    case 40: return "E2";
    case 41: return "F2";
    case 42: return "F#2";
    case 43: return "G2";
    case 44: return "G#2";
    case 45: return "A2";
    case 46: return "A#2";
    case 47: return "B2";
    case 48: return "C3";
    case 49: return "C#3";
    case 50: return "D3";
    case 51: return "D#3";
    case 52: return "E3";
    case 53: return "F3";
    case 54: return "F#3";
    case 55: return "G3";
    case 56: return "G#3";
    case 57: return "A3";
    case 58: return "A#3";
    case 59: return "B3";
    case 60: return "C4";
    case 61: return "C#4";
    case 62: return "D4";
    case 63: return "D#4";
    case 64: return "E4";
    case 65: return "F4";
    case 66: return "F#4";
    case 67: return "G4";
    case 68: return "G#4";
    case 69: return "A4";
    case 70: return "A#4";
    case 71: return "B4";
    case 72: return "C5";
    case 73: return "C#5";
    case 74: return "D5";
    case 75: return "D#5";
    case 76: return "E5";
    case 77: return "F5";
    case 78: return "F#5";
    case 79: return "G5";
    case 80: return "G#5";
    case 81: return "A5";
    case 82: return "A#5";
    case 83: return "B5";
    case 84: return "C6";
    case 85: return "C#6";
    case 86: return "D6";
    case 87: return "D#6";
    case 88: return "E6";
    case 89: return "F6";
    case 90: return "F#6";
    case 91: return "G6";
    case 92: return "G#6";
    case 93: return "A6";
    case 94: return "A#6";
    case 95: return "B6";
    case 96: return "C7";
    case 97: return "C#7";
    case 98: return "D7";
    case 99: return "D#7";
    case 100: return "E7";
    case 101: return "F7";
    case 102: return "F#7";
    case 103: return "G7";
    case 104: return "G#7";
    case 105: return "A7";
    case 106: return "A#7";
    case 107: return "B7";
    case 108: return "C8";
    case 109: return "C#8";
    case 110: return "D8";
    case 111: return "D#8";
    case 112: return "E8";
    case 113: return "F8";
    case 114: return "F#8";
    case 115: return "G8";
    case 116: return "G#8";
    case 117: return "A8";
    case 118: return "A#8";
    case 119: return "B8";
    case 120: return "C9";
    case 121: return "C#9";
    case 122: return "D9";
    case 123: return "D#9";
    case 124: return "E9";
    case 125: return "F9";
    case 126: return "F#9";
    case 127: return "G9";

    default:  return "Invalid MIDI note";
  }
}

const char* eMidi_drumToStr(uint8_t midiNote) {
  switch(midiNote) {
    case 35: return "Acoustic Bass Drum";
    case 36: return "Bass Drum 1";
    case 37: return "Side Stick";
    case 38: return "Acoustic Snare";
    case 39: return "Hand Clap";
    case 40: return "Electric Snare";
    case 41: return "Low Floor Tom";
    case 42: return "Closed Hi Hat";
    case 43: return "High Floor Tom";
    case 44: return "Pedal Hi-Hat";
    case 45: return "Low Tom";
    case 46: return "Open Hi-Hat";
    case 47: return "Low-Mid Tom";
    case 48: return "Hi Mid Tom";
    case 49: return "Crash Cymbal 1";
    case 50: return "High Tom";
    case 51: return "Ride Cymbal 1";
    case 52: return "Chinese Cymbal";
    case 53: return "Ride Bell";
    case 54: return "Tambourine";
    case 55: return "Splash Cymbal";
    case 56: return "Cowbell";
    case 57: return "Crash Cymbal 2";
    case 58: return "Vibraslap";
    case 59: return "Ride Cymbal 2";
    case 60: return "Hi Bongo";
    case 61: return "Low Bongo";
    case 62: return "Mute Hi Conga";
    case 63: return "Open Hi Conga";
    case 64: return "Low Conga";
    case 65: return "High Timbale";
    case 66: return "Low Timbale";
    case 67: return "High Agogo";
    case 68: return "Low Agogo";
    case 69: return "Cabasa";
    case 70: return "Maracas";
    case 71: return "Short Whistle";
    case 72: return "Long Whistle";
    case 73: return "Short Guiro";
    case 74: return "Long Guiro";
    case 75: return "Claves";
    case 76: return "Hi Wood Block";
    case 77: return "Low Wood Block";
    case 78: return "Mute Cuica";
    case 79: return "Open Cuica";
    case 80: return "Mute Triangle";
    case 81: return "Open Triangle";

    // Non standard:
    case 82: return "Shaker";
    case 83: return "Sleigh Bell";
    case 84: return "Bell Tree";
    case 85: return "Castanets";
    case 86: return "Surdu Dead Stroke";
    case 87: return "Surdu";
    case 91: return "Snare Drum Rod";
    case 92: return "Ocean Drum";
    case 93: return "Snare Drum Brush";

    default: return "Unknown percussion";
  }
}

const char* eMidi_controllerToStr(uint8_t controllerNo) {
  switch(controllerNo) {
    case 0:   return "Bank Select";
    case 1:   return "Modulation";
    case 2:   return "Breath Controller";
    case 4:   return "Foot Controller";
    case 5:   return "Portamento Time";
    case 6:   return "Data Entry MSB";
    case 7:   return "Volume";
    case 8:   return "Balance";
    case 10:  return "Pan";
    case 11:  return "Expression";
    case 12:  return "Effect Control 1";
    case 13:  return "Effect Control 2";
    case 16:  return "General Purpose Controller 1";
    case 17:  return "General Purpose Controller 2";
    case 18:  return "General Purpose Controller 3";
    case 19:  return "General Purpose Controller 4";
    case 32:  return "LSB for Control 0 (Bank Select)";
    case 33:  return "LSB for Control 1 (Modulation Wheel or Lever)";
    case 34:  return "LSB for Control 2 (Breath Controller)";
    case 36:  return "LSB for Control 4 (Foot Controller)";
    case 37:  return "LSB for Control 5 (Portamento Time)";
    case 38:  return "LSB for Control 6 (Data Entry)";
    case 39:  return "LSB for Control 7 (Channel Volume)";
    case 40:  return "LSB for COntrol 8 (Balance)";
    case 42:  return "LSB for Control 10 (Pan)";
    case 43:  return "LSB for Control 11 (Expression Controller)";
    case 44:  return "LSB for Control 12 (Effect Control 1)";
    case 45:  return "LSB for Control 13 (Effect Control 2)";
    case 48:  return "LSB for Control 16 General Purpose Controller 1";
    case 49:  return "LSB for Control 16 General Purpose Controller 2";
    case 50:  return "LSB for Control 16 General Purpose Controller 3";
    case 51:  return "LSB for Control 16 General Purpose Controller 4";
    case 64:  return "Sustain";
    case 65:  return "Portamento On/Off";
    case 66:  return "Sostenuto On/Off";
    case 67:  return "Soft Pedal On/Off";
    case 68:  return "Legato Footswitch";
    case 69:  return "Hold 2";
    case 70:  return "Sound Controller 1";
    case 71:  return "Sound Controller 2";
    case 72:  return "Sound Controller 3";
    case 73:  return "Sound Controller 4";
    case 74:  return "Sound Controller 5";
    case 75:  return "Sound Controller 6";
    case 76:  return "Sound Controller 7";
    case 77:  return "Sound Controller 8";
    case 78:  return "Sound Controller 9";
    case 79:  return "Sound Controller 10";
    case 80:  return "General Purpose Controller 5";
    case 81:  return "General Purpose Controller 6";
    case 82:  return "General Purpose Controller 7";
    case 83:  return "General Purpose Controller 8";
    case 84:  return "Portamento Control";
    case 88:  return "High Resolution Velocity Prefix";
    case 91:  return "Effect 1 Depth";
    case 92:  return "Effect 2 Depth";
    case 93:  return "Effect 3 Depth";
    case 94:  return "Effect 4 Depth";
    case 95:  return "Effect 5 Depth";
    case 96:  return "Data Increment";
    case 97:  return "Data Decrement";
    case 120: return "All Sound Off";
    case 121: return "Reset All Controllers";
    case 122: return "Local Control On/Off";
    case 123: return "All Notes Off";
    case 124: return "Omni Mode Off";
    case 125: return "Omni Mode On";
    case 126: return "Mono Mode On";
    case 127: return "Poly Mode On";

    default: return "Unknown controller";
  }
}

const char* eMidi_programToStr(uint8_t programNo) {
  switch(programNo + 1) {
    case   1: return "Acoustic Grand Piano";
    case   2: return "Bright Acoustic Piano";
    case   3: return "Electric Grand Piano";
    case   4: return "Honky-tonk Piano";
    case   5: return "Electric Piano 1";
    case   6: return "Electric Piano 2";
    case   7: return "Harpsichord";
    case   8: return "Clavi";
    case   9: return "Celesta";
    case  10: return "Glockenspiel";
    case  11: return "Music Box";
    case  12: return "Vibraphone";
    case  13: return "Marimba";
    case  14: return "Xylophone";
    case  15: return "Tubular Bells";
    case  16: return "Dulcimer";
    case  17: return "Drawbar Organ";
    case  18: return "Percussive Organ";
    case  19: return "Rock Organ";
    case  20: return "Church Organ";
    case  21: return "Reed Organ";
    case  22: return "Accordion";
    case  23: return "Harmonica";
    case  24: return "Tango Accordion";
    case  25: return "Acoustic Guitar (nylon)";
    case  26: return "Acoustic Guitar (steel)";
    case  27: return "Electric Guitar (jazz)";
    case  28: return "Electric Guitar (clean)";
    case  29: return "Electric Guitar (muted)";
    case  30: return "Overdriven Guitar";
    case  31: return "Distortion Guitar";
    case  32: return "Guitar harmonics";
    case  33: return "Acoustic Bass";
    case  34: return "Electric Bass (finger)";
    case  35: return "Electric Bass (pick)";
    case  36: return "Fretless Bass";
    case  37: return "Slap Bass 1";
    case  38: return "Slap Bass 2";
    case  39: return "Synth Bass 1";
    case  40: return "Synth Bass 2";
    case  41: return "Violin";
    case  42: return "Viola";
    case  43: return "Cello";
    case  44: return "Contrabass";
    case  45: return "Tremolo Strings";
    case  46: return "Pizzicato Strings";
    case  47: return "Orchestral Harp";
    case  48: return "Timpani";
    case  49: return "String Ensemble 1";
    case  50: return "String Ensemble 2";
    case  51: return "SynthStrings 1";
    case  52: return "SynthStrings 2";
    case  53: return "Choir Aahs";
    case  54: return "Voice Oohs";
    case  55: return "Synth Voice";
    case  56: return "Orchestra Hit";
    case  57: return "Trumpet";
    case  58: return "Trombone";
    case  59: return "Tuba";
    case  60: return " Muted Trumpet";
    case  61: return "French Horn";
    case  62: return "Brass Section";
    case  63: return "SynthBrass 1";
    case  64: return "SynthBrass 2";
    case  65: return "Soprano Sax";
    case  66: return "Alto Sax";
    case  67: return "Tenor Sax";
    case  68: return "Baritone Sax";
    case  69: return "Oboe";
    case  70: return "English Horn";
    case  71: return "Bassoon";
    case  72: return "Clarinet";
    case  73: return "Piccolo";
    case  74: return "Flute";
    case  75: return "Recorder";
    case  76: return "Pan Flute";
    case  77: return "Blown Bottle";
    case  78: return "Shakuhachi";
    case  79: return "Whistle";
    case  80: return "Ocarina";
    case  81: return "Lead 1 (square)";
    case  82: return "Lead 2 (sawtooth)";
    case  83: return "Lead 3 (calliope)";
    case  84: return "Lead 4 (chiff)";
    case  85: return "Lead 5 (charang)";
    case  86: return "Lead 6 (voice)";
    case  87: return "Lead 7 (fifths)";
    case  88: return "Lead 8 (bass + lead)";
    case  89: return "Pad 1 (new age)";
    case  90: return "Pad 2 (warm)";
    case  91: return "Pad 3 (polysynth)";
    case  92: return "Pad 4 (choir)";
    case  93: return "Pad 5 (bowed)";
    case  94: return "Pad 6 (metallic)";
    case  95: return "Pad 7 (halo)";
    case  96: return "Pad 8 (sweep)";
    case  97: return "FX 1 (rain)";
    case  98: return "FX 2 (soundtrack)";
    case  99: return "FX 3 (crystal)";
    case 100: return "FX 4 (atmosphere)";
    case 101: return "FX 5 (brightness)";
    case 102: return "FX 6 (goblins)";
    case 103: return "FX 7 (echoes)";
    case 104: return "FX 8 (sci-fi)";
    case 105: return "Sitar";
    case 106: return "Banjo";
    case 107: return "Shamisen";
    case 108: return "Koto";
    case 109: return "Kalimba";
    case 110: return "Bag pipe";
    case 111: return "Fiddle";
    case 112: return "Shanai";
    case 113: return "Tinkle Bell";
    case 114: return "Agogo";
    case 115: return "Steel Drums";
    case 116: return "Woodblock";
    case 117: return "Taiko Drum";
    case 118: return "Melodic Tom";
    case 119: return "Synth Drum";
    case 120: return "Reverse Cymbal";
    case 121: return "Guitar Fret Noise";
    case 122: return "Breath Noise";
    case 123: return "Seashore";
    case 124: return "Bird Tweet";
    case 125: return "Telephone Ring";
    case 126: return "Helicopter";
    case 127: return "Applause";
    case 128: return "Gunshot";

    default: return "Unknown program";
  }
}

static uint32_t prvUspqn2bpm(uint32_t uspqn) {
  static const uint32_t c = 60000000;

  return c / uspqn;
}

static void printMidiNote(uint8_t channel, uint8_t midiNote, uint8_t velocity) {
  if(channel == 9)
    eMidi_printf(", Drum: %s (%d), Velocity: %d", eMidi_drumToStr(midiNote), midiNote, velocity);
  else
    eMidi_printf(", Note: %s (%d), Velocity: %d", eMidi_numberToNote(midiNote), midiNote, velocity);
}

Error eMidi_printMidiEvent(const MidiEvent* e) {
  eMidi_printf("[0x%02X%s] %s", e->eventId, e->isRunningStatus ? " R" : "",
      eMidi_eventToStr(e->eventId));

  uint8_t channelNo      = e->eventId & 0x0F;
  uint8_t channelMessage = e->eventId & 0xF0;
  uint8_t systemMessage  = e->eventId;

  switch(channelMessage) {
    case MIDI_EVENT_NOTE_OFF:
      printMidiNote(channelNo, e->params.msg.noteOff.note, e->params.msg.noteOff.velocity);
      break;

    case MIDI_EVENT_NOTE_ON:
      printMidiNote(channelNo, e->params.msg.noteOn.note, e->params.msg.noteOn.velocity);

      if(e->params.msg.noteOn.velocity == 0)
        eMidi_printf(" (Note-Off)");

      break;

    case MIDI_EVENT_POLY_KEY_PRESSURE:
      break;

    case MIDI_EVENT_CONTROL_CHANGE:
      eMidi_printf(", Controller: %s (%d), value: %d", eMidi_controllerToStr(e->params.msg.controlChange.control),
          e->params.msg.controlChange.control, e->params.msg.controlChange.value);
      break;

    case MIDI_EVENT_PROGRAM_CHANGE:
      eMidi_printf(", Program: '%s' (%d)", eMidi_programToStr(e->params.msg.programChange.programNumber),
          e->params.msg.programChange.programNumber);

      break;

    case MIDI_EVENT_CHANNEL_PRESSURE:
      break;

    case MIDI_EVENT_PITCH_BEND:
      eMidi_printf(", Value: %d", e->params.msg.pitchBend.value);
      break;
  }

  if(e->eventId == MIDI_EVENT_META) {
    eMidi_printf(" - [0x%02x] %s ", e->metaEventId, eMidi_metaEventToStr(e->metaEventId));

    switch(e->metaEventId) {
      case MIDI_SET_TEMPO:
        eMidi_printf("(%d bpm)", prvUspqn2bpm(e->params.msg.meta.setTempo.usPerQuarterNote));
        break;

      case MIDI_META_MIDI_PORT:
        eMidi_printf("(port: %d)", e->params.msg.meta.midiPort.port);
        break;

      default:
        break;
    }
  }

  eMidi_printf("\n");

  return EMIDI_OK;
}

Error eMidi_printError(Error error) {
  eMidi_printf("Error %d: %s\n", error, eMidi_errorToStr(error));

  return EMIDI_OK;
}

Error eMidi_printFileInfo(const MidiFile* pMidiFile) {
  eMidi_printf("File size: %d bytes\n", pMidiFile->size);
  eMidi_printf("\n");
  eMidi_printf("MIDI header:\n");
  eMidi_printf("------------\n");
  eMidi_printf("File format: %d\n", pMidiFile->header.format);
  eMidi_printf("Number of tracks: %d\n", pMidiFile->header.ntrks);
  eMidi_printf("Division format: %s\n", pMidiFile->header.format == 0 ? "TPQN" : "SMPTE");

  switch(pMidiFile->header.division.format) {
    case DIVISION_TPQN:
      eMidi_printf("Ticks per quarter note: %d\n", pMidiFile->header.division.tqpn.TQPN);
      break;

    case DIVISION_SMPTE:
      eMidi_printf("SMPTE format is not supported!\n");
      break;
  }

  eMidi_printf("\n");

  return EMIDI_OK;
}

