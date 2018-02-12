#include "midifile.h"
#include "helpers.h"

const char* eMidi_errorToStr(Error error) {
  switch(error) {
    case EMIDI_OK:                      return "Ok";
    case EMIDI_OK_END_OF_FILE:          return "Ok, end of file";
    case EMIDI_INVALID_HANDLE:          return "Invalid Handle";
    case EMIDI_CANNOT_OPEN_FILE:        return "Cannot open file";
    case EMIDI_INVALID_MIDI_FILE:       return "Invalid MIDI file";
    case EMIDI_UNEXPECTED_END_OF_FILE:  return "Unexpected end of File";
    case EMIDI_SYNTAX_ERROR:            return "Syntax Error";
    case EMIDI_FORMAT_1_NOT_SUPPORTED:  return "Format 1 not supported";
    case EMIDI_FORMAT_2_NOT_SUPPORTED:  return "Format 2 not supported";

    default:                            return "Unknown";
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
    case MIDI_END_OF_TRACK:             return "End of Track";
    case MIDI_SET_TEMPO:                return "Set Tempo";
    case MIDI_SMPTE_OFFSET:             return "SMPTE Offset";
    case MIDI_TIME_SIGNATURE:           return "Time Signature";
    case MIDI_KEY_SIGNATURE:            return "Key Signature";
    case MIDI_SEQUENCER_SPECIFIC:       return "Sequencer Specific";

    default:                            return "Unknown";
  }
}

static uint32_t prvUspqn2bpm(uint32_t uspqn) {
  static const uint32_t c = 60000000;

  return c / uspqn;
}

Error eMidi_printMidiEvent(const MidiEvent* e) {
  printf("[%04d] [0x%02X%s] %s", e->deltaTime, e->eventId, e->isRunningStatus ? " R" : "",
      eMidi_eventToStr(e->eventId));
  uint8_t channelMessage = e->eventId & 0xF0;
  uint8_t systemMessage  = e->eventId;

  switch(channelMessage) {
    case MIDI_EVENT_NOTE_OFF:
      // TODO: implement number to note function
      printf(", Note: %d, Velocity: %d", e->params.noteOff.note, e->params.noteOff.velocity);
      break;

    case MIDI_EVENT_NOTE_ON:
      // TODO: implement number to note function
      printf(", Note: %d, Velocity: %d", e->params.noteOn.note, e->params.noteOn.velocity);

      if(e->params.noteOn.velocity == 0)
        printf(" (Note-Off)");

      break;

    case MIDI_EVENT_POLY_KEY_PRESSURE:
      break;

    case MIDI_EVENT_CONTROL_CHANGE:
      break;

    case MIDI_EVENT_PROGRAM_CHANGE:
      // TODO: print instrument
      break;

    case MIDI_EVENT_CHANNEL_PRESSURE:
      break;

    case MIDI_EVENT_PITCH_BEND:
      printf(", Value: %d", e->params.pitchBend.value);
      break;
  }

  if(e->eventId == MIDI_EVENT_META) {
    printf(" - [0x%02x] %s ", e->metaEventId, eMidi_metaEventToStr(e->metaEventId));

    switch(e->metaEventId) {
      case MIDI_SET_TEMPO:
        printf("(%d bpm)", prvUspqn2bpm(e->params.meta.setTempo.usPerQuarterNote));
        break;

      default:
        break;
    }
  }

  printf("\n");

  return EMIDI_OK;
}

Error eMidi_printError(Error error) {
  printf("Error %d: %s\n", error, eMidi_errorToStr(error));

  return EMIDI_OK;
}

Error eMidi_printFileInfo(MidiFile* pMidiFile) {
  printf("File size: %d bytes\n", pMidiFile->size);
  printf("\n");
  printf("MIDI header:\n");
  printf("------------\n");
  printf("File format: %d\n", pMidiFile->header.format);
  printf("Number of tracks: %d\n", pMidiFile->header.ntrks);
  printf("Division format: %s\n", pMidiFile->header.format == 0 ? "TPQN" : "SMPTE");

  switch(pMidiFile->header.division.format) {
    case DIVISION_TPQN:
      printf("Ticks per quarter note: %d\n", pMidiFile->header.division.tqpn.TQPN);
      break;

    case DIVISION_SMPTE:
      printf("SMPTE format is not supported!\n");
      break;
  }

  printf("\n");

  return EMIDI_OK;
}

