#ifndef _MIDIFILE_H
#define _MIDIFILE_H

#include <stdint.h>
#include <stdio.h>

typedef struct MidiChunkInfo {
  char pChunk[4];
  uint32_t length;
} MidiChunkInfo;

typedef struct MidiHeader { 
  uint16_t format;
  uint16_t ntrks;
  uint16_t division;  
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
} MidiFile;

typedef uint32_t Error;

enum {
  EMIDI_OK                     = 0,
  EMIDI_INVALID_HANDLE         = 1,
  EMIDI_CANNOT_OPEN_FILE       = 2,
  EMIDI_INVALID_MIDI_FILE      = 3,
  EMIDI_UNEXPECTED_END_OF_FILE = 4,
 
// remove as soon as supported: 
  EMIDI_FORMAT_1_NOT_SUPPORTED = 1000,
  EMIDI_FORMAT_2_NOT_SUPPORTED = 1001
};

Error eMidi_open(MidiFile* pMidiFile, const char* pFileName);
Error eMidi_close(MidiFile* pMidiFile);

#endif // _MIDIFILE_H

