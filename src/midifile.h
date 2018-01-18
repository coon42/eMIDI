#ifndef _MIDIFILE_H
#define _MIDIFILE_H

#include <stdint.h>
#include <stdio.h>

typedef struct MidiFile {
  FILE* p;
  uint32_t size;
} MidiFile;

typedef struct MidiHeaderChunk {
  char chunk[4]; // Must be MThd
  uint32_t length;
  uint8_t format;
  uint8_t ntrks;
  uint8_t division;  
} MidiHeaderChunk;

typedef uint32_t Error;

enum {
  EMIDI_OK                = 0,
  EMIDI_INVALID_HANDLE    = 1,
  EMIDI_CANNOT_OPEN_FILE  = 2,
  EMIDI_INVALID_MIDI_FILE = 3
};

Error eMidi_open(struct MidiFile* pMidiFile, const char* pFileName);
Error eMidi_close(struct MidiFile* pMidiFile);

#endif // _MIDIFILE_H

