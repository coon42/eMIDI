#include <string.h>
#include "midifile.h"
#include "byteswap.h"

static Error prvReadVoid(FILE* p, void* pOut, int len, int* pNumBytesRead) {
  int n = fread(pOut, 1, len, p);

  if(pNumBytesRead)
    *pNumBytesRead = n;

  if(n < len)
    return EMIDI_UNEXPECTED_END_OF_FILE;

  return EMIDI_OK;
}

static Error prvReadByte(FILE* p, uint8_t* pOut, int* pNumBytesRead) {
  return prvReadVoid(p, pOut, sizeof(uint8_t), pNumBytesRead);
}

static Error prvReadWord(FILE* p, uint16_t* pOut, int* pNumBytesRead) {
  return prvReadVoid(p, pOut, sizeof(uint16_t*), pNumBytesRead);
}

static Error prvReadDword(FILE* p, uint32_t* pOut, int* pNumBytesRead) {
  return prvReadVoid(p, pOut, sizeof(uint32_t*), pNumBytesRead);
}

Error eMidi_open(MidiFile* pMidiFile, const char* pFileName) {
  if(!pMidiFile)
    return EMIDI_INVALID_HANDLE;

  FILE* p = fopen(pFileName, "rb");

  if(!p)
    return EMIDI_CANNOT_OPEN_FILE;

  uint32_t fileSize;;
  fseek(p, 0, SEEK_END);
  fileSize = ftell(p);
  fseek(p, 0, SEEK_SET); 

  MidiHeaderChunk header;
  int32_t numBytesRead;

  Error error; 
  error = prvReadVoid(p, &header, sizeof(MidiHeaderChunk), &numBytesRead);

  if(error)
    return error;

  if(memcmp(header.pChunk, "MThd", 4) != 0)
    return EMIDI_INVALID_MIDI_FILE;

  header.length   = __bswap_32(header.length);
  header.format   = __bswap_16(header.format);
  header.ntrks    = __bswap_16(header.ntrks);
  header.division = __bswap_16(header.division);

  if(header.length != 6)
    return EMIDI_INVALID_MIDI_FILE;

  if(header.format > 2)
    return EMIDI_INVALID_MIDI_FILE;

  if(header.format == 1)
    return EMIDI_FORMAT_1_NOT_SUPPORTED;

  if(header.format == 2)
    return EMIDI_FORMAT_2_NOT_SUPPORTED;

  pMidiFile->p = p;
  pMidiFile->size = fileSize;
  pMidiFile->header = header;

  return EMIDI_OK;
}

Error eMidi_close(MidiFile* pMidiFile) {
  if(!pMidiFile)
    return EMIDI_INVALID_HANDLE;

  fclose(pMidiFile->p);

  return EMIDI_OK;
}

