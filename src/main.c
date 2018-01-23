#include "midifile.h"

static const char* errorToStr(Error error) {
  switch(error) {
    case EMIDI_OK:                      return "Ok";
    case EMIDI_INVALID_HANDLE:          return "Invalid Handle";
    case EMIDI_CANNOT_OPEN_FILE:        return "Cannot open file";
    case EMIDI_INVALID_MIDI_FILE:       return "Invalid MIDI file";
    case EMIDI_UNEXPECTED_END_OF_FILE:  return "Unexpected end of File";
    case EMIDI_FEATURE_NOT_IMPLEMENTED: return "Feature not implemented";
    case EMIDI_FORMAT_1_NOT_SUPPORTED:  return "Format 1 not supportd";
    case EMIDI_FORMAT_2_NOT_SUPPORTED:  return "Format 2 not supportd";

    default:                            return "Unknown";
  }
}

static const char* midiEventToStr(int eventCode) {
  switch(eventCode) {
    case MIDI_EVENT_NOTE_ON:                 return "Note-Off";
    case MIDI_EVENT_NOTE_OFF:                return "Note-On";
    case MIDI_EVENT_POLY_KEY_PRESSURE:       return "Poly Key Pressure";
    case MIDI_EVENT_CONTROL_CHANGE:          return "Control Change";
    case MIDI_EVENT_PROGRAM_CHANGE:          return "Program Change";
    case MIDI_EVENT_CHANNEL_PRESSURE:        return "Channel Pressure";
    case MIDI_EVENT_PITCH_BEND:              return "Pitch Bend";
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

static const char* metaEventToStr(int metaEventId) {
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
    case MIDI_END_OF_TRACK:             return "End of Track";
    case MIDI_SET_TEMPO:                return "Set Tempo";
    case MIDI_SMPTE_OFFSET:             return "SMPTE Offset";
    case MIDI_TIME_SIGNATURE:           return "Time Signature";
    case MIDI_KEY_SIGNATURE:            return "Key Signature";
    case MIDI_SEQUENCER_SPECIFIC:       return "Sequencer Specific";

    default:                            return "Unknown";
  }
}

static void printError(Error error) {
  printf("Error %d: %s\n", error, errorToStr(error));
}

static void printMidiFileInfo(MidiFile* pMidiFile) {
  printf("File size: %d bytes\n", pMidiFile->size);
  printf("\n");
  printf("MIDI header:\n");
  printf("------------\n");
  printf("Format: %d\n", pMidiFile->header.format);
  printf("Number of Tracks: %d\n", pMidiFile->header.ntrks);
  printf("Division: %d\n", pMidiFile->header.division);
  printf("\n");
}

static void printMidiFileEvents(MidiFile* pMidiFile) {
  printf("Now reading MIDI events:\n");
  printf("========================\n\n");
  MidiEvent e;
  Error error;

  do {
    if(error = eMidi_readEvent(pMidiFile, &e)) {
      printf("Error on reading event: %d (%s)\n", error, errorToStr(error));
      return;
    }

    printf("Next event is: 0x%02X (%s)", e.eventId,
        midiEventToStr(e.eventId));

    if(e.eventId == MIDI_EVENT_META)
      printf(" - %s", metaEventToStr(e.metaEventId));
  
    printf("\n");
 
  } while (error == EMIDI_OK);
}

int main(int argc, char* pArgv[]) {
  if(argc < 2) {
    printf("Usage: eMidi.o <midi file>");

    return 1;
  }

  const char* pMidiFileName = pArgv[1];
  MidiFile midi;  
  Error error;

  error = eMidi_open(&midi, pMidiFileName);

  if(error) {
    printf("Cannot open file: '%s'\n", pMidiFileName);
    printError(error);

    return 2;
  }

  printf("Midi file '%s' opened successfully!\n", pMidiFileName);
  printMidiFileInfo(&midi);
  printMidiFileEvents(&midi);

  eMidi_close(&midi);

  return 0;
}

