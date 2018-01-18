#include "midifile.h"
#include "byteswap.h"

Error eMidi_open(MidiFile* pMidiFile, const char* pFileName) {
  if(!pMidiFile)
    return EMIDI_INVALID_HANDLE;

  FILE* p = fopen(pFileName, "rb");

  if(!p)
    return EMIDI_CANNOT_OPEN_FILE;

  // Get file size of midi file:
  uint32_t fileSize;;
  fseek(p, 0, SEEK_END);
  fileSize = ftell(p);
  fseek(p, 0, SEEK_SET); 

  // TODO: improve file validity checks:
  if(fileSize < sizeof(MidiHeaderChunk))
    return EMIDI_INVALID_MIDI_FILE;

  MidiHeaderChunk header;
  int32_t numBytesRead;

  numBytesRead = fread(&header, 1, sizeof(MidiHeaderChunk), p);

  if(numBytesRead < sizeof(MidiHeaderChunk))
    return EMIDI_INVALID_MIDI_FILE;

  // Big engian to little endian conversion (make configurable?):
  header.length   = __bswap_32(header.length);
  header.format   = __bswap_16(header.format);
  header.ntrks    = __bswap_16(header.ntrks);
  header.division = __bswap_16(header.division);
  // - 

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

