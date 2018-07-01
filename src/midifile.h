#ifndef _MIDIFILE_H
#define _MIDIFILE_H

#include <stdbool.h>
#include <stdio.h>
#include "emiditypes.h"

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

typedef enum MidiFileMode {
  MIDI_FILE_MODE_READ,
  MIDI_FILE_MODE_CREATE
} MidiFileMode;

typedef struct MidiStatusParams {
  uint8_t pRaw[2];

  union {
    struct {
      uint8_t note;
      uint8_t velocity;
    } noteOff;

    struct {
      uint8_t note;
      uint8_t velocity;
    } noteOn;

    struct {
      uint8_t control;
      uint8_t value;
    } controlChange;

    struct {
      uint8_t programNumber;
    } programChange;

    struct {
      uint16_t value;
    } pitchBend;

    struct {
      struct {
        uint32_t usPerQuarterNote;
      } setTempo;

      struct {
        uint8_t port;
      } midiPort;
    } meta;
  } msg;

} MidiStatusParams;

typedef struct MidiEvent {
  uint32_t deltaTime;
  uint8_t eventId;
  uint8_t metaEventId;
  uint8_t metaEventLen;
  MidiStatusParams params;
  bool isRunningStatus;
} MidiEvent;

typedef struct MidiEventList {
  MidiEvent event;
  struct MidiEventList* pNext;
} MidiEventList;

typedef struct MidiFile {
  FILE* p;
  MidiFileMode mode;
  MidiHeader header;

  // Only used in read mode:
  uint32_t size;
  MidiTrack track;
  uint8_t prevEventId;

  // Only used in write mode:
  MidiEventList* pEventList;
} MidiFile;

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
  MIDI_META_MIDI_PORT           = 0x21, // Non-Standard
  MIDI_END_OF_TRACK             = 0x2F,
  MIDI_SET_TEMPO                = 0x51,
  MIDI_SMPTE_OFFSET             = 0x54,
  MIDI_TIME_SIGNATURE           = 0x58,
  MIDI_KEY_SIGNATURE            = 0x59,
  MIDI_SEQUENCER_SPECIFIC       = 0x7F
};

// Read-API:
Error eMidi_open(MidiFile* pMidiFile, const char* pFileName);
Error eMidi_readEvent(MidiFile* pMidiFile, MidiEvent* pEvent);

// Write-API:
Error eMidi_create(MidiFile* pMidiFile);
Error eMidi_writeNoteOnEvent(MidiFile* pMidiFile, uint32_t deltaTime, uint8_t channel, uint8_t note, uint8_t velocity);
Error eMidi_writeControlChangeEvent(MidiFile* pMidiFile, uint32_t deltaTime, uint8_t channel, uint8_t control, uint8_t value);
Error eMidi_writeProgramChangeEvent(MidiFile* pMidiFile, uint32_t deltaTime, uint8_t channel, uint8_t programNumber);
Error eMidi_writeSetTempoMetaEvent(MidiFile* pMidiFile, uint32_t deltaTime, uint32_t bpm);
Error eMidi_save(MidiFile* pMidiFile, const char* pFileName);

// Common:
Error eMidi_close(MidiFile* pMidiFile);

#endif // _MIDIFILE_H

