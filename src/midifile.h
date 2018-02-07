#ifndef _MIDIFILE_H
#define _MIDIFILE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct MidiChunkInfo {
  char pChunk[4];
  uint32_t length;
} MidiChunkInfo;

enum DivisionFormat {
  DIVISION_TPQN,
  DIVISION_SMPTE
};

typedef struct MidiHeader {
  uint16_t format;
  uint16_t ntrks;

  union {
    uint16_t raw;

    uint16_t
           : 15,
    format : 1;

    struct {
      uint16_t
      ticksPerFrame  : 8,
      negSmpteFormat : 7;
    } smpte;

    struct {
      uint16_t
      TQPN : 15;
    } tqpn;
  } division;

} MidiHeader;

typedef struct MidiTrack {
  uint32_t startPos;
  uint32_t pos;
  int len;
} MidiTrack;

typedef struct MidiFile {
  FILE* p;
  uint32_t size;
  MidiHeader header;
  MidiTrack track;
  uint8_t prevEventId;
} MidiFile;

typedef union MidiStatusParams {
  struct {
    uint8_t note;
    uint8_t velocity;
  } noteOn;

  struct {
    uint8_t note;
    uint8_t velocity;
  } noteOff;
} MidiStatusParams;

typedef struct MidiEvent {
  uint32_t deltaTime;
  uint8_t eventId;
  uint8_t metaEventId;
  uint8_t metaEventLen;
  MidiStatusParams params;
  bool isRunningStatus;
} MidiEvent;

enum MidiVoiceMessages {
  MIDI_EVENT_NOTE_OFF          = 0x80,
  MIDI_EVENT_NOTE_ON           = 0x90,
  MIDI_EVENT_POLY_KEY_PRESSURE = 0xA0,
  MIDI_EVENT_CONTROL_CHANGE    = 0xB0,
  MIDI_EVENT_PROGRAM_CHANGE    = 0xC0,
  MIDI_EVENT_CHANNEL_PRESSURE  = 0xD0,
  MIDI_EVENT_PITCH_BEND        = 0xE0
};

enum MidiMetaEvent {
  MIDI_EVENT_META              = 0xFF
};

enum SystemExclusiveMessages {
  MIDI_EVENT_SYSTEM_EXCLUSIVE = 0xF0
};

enum SystemCommonMessages {
  MIDI_EVENT_TIME_CODE_QUARTER_FRAME = 0xF1,
  MIDI_EVENT_SONG_POSITION_POINTER   = 0xF2,
  MIDI_EVENT_SONG_SELECT             = 0xF3,
  MIDI_EVENT_TUNE_REQUEST            = 0xF6,
  MIDI_EVENT_END_OF_EXCLUSIVE        = 0xF7
};

enum SystemRealTimeMessages {
  MIDI_EVENT_TIMING_CLOCK   = 0xF8,
  MIDI_EVENT_START          = 0xFA,
  MIDI_EVENT_CONTINUE       = 0xFB,
  MIDI_EVENT_STOP           = 0xFC,
  MIDI_EVENT_ACTIVE_SENSING = 0xFE,
  MIDI_EVENT_SYSTEM_RESET   = 0xFF
};

enum MetaEvents {
  MIDI_META_SEQUENCE_NUMBER     = 0x00,
  MIDI_META_TEXT_EVENT          = 0x01,
  MIDI_META_COPYRIGHT_NOTICE    = 0x02,
  MIDI_META_SEQUENCE_TRACK_NAME = 0x03,
  MIDI_META_INSTRUMENT_NAME     = 0x04,
  MIDI_META_LYRIC               = 0x05,
  MIDI_META_MARKER              = 0x06,
  MIDI_META_CUE_POINT           = 0x07,
  // 0x08 .. 0x0F reserved
  MIDI_META_MIDI_CHANNEL_PREFIX = 0x20,
  MIDI_END_OF_TRACK             = 0x2F,
  MIDI_SET_TEMPO                = 0x51,
  MIDI_SMPTE_OFFSET             = 0x54,
  MIDI_TIME_SIGNATURE           = 0x58,
  MIDI_KEY_SIGNATURE            = 0x59,
  MIDI_SEQUENCER_SPECIFIC       = 0x7F
};

typedef uint32_t Error;

enum {
  EMIDI_OK                     = 0x0000,
  EMIDI_OK_END_OF_FILE         = 0x0001,

  EMIDI_INVALID_HANDLE         = 0x1000,
  EMIDI_CANNOT_OPEN_FILE       = 0x1001,
  EMIDI_INVALID_MIDI_FILE      = 0x1002,
  EMIDI_SYNTAX_ERROR           = 0x1003,
  EMIDI_UNEXPECTED_END_OF_FILE = 0x1004,

// remove as soon as supported:
  EMIDI_FORMAT_1_NOT_SUPPORTED        = 0x2000,
  EMIDI_FORMAT_2_NOT_SUPPORTED        = 0x2001,
  EMIDI_DIVISION_FORMAT_NOT_SUPPORTED = 0x2002
};

Error eMidi_open(MidiFile* pMidiFile, const char* pFileName);
Error eMidi_close(MidiFile* pMidiFile);
Error eMidi_readEvent(MidiFile* pMidiFile, MidiEvent* pEvent);

#endif // _MIDIFILE_H

