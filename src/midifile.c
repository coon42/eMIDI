#include "midifile.h"

Error eMidi_open(struct MidiFile* pMidiFile, const char* pFileName) {
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
  if(fileSize < 6)
    return EMIDI_INVALID_MIDI_FILE;

  // TODO: read header 

  pMidiFile->p = p;
  pMidiFile->size = fileSize;

  return EMIDI_OK;
}

Error eMidi_close(struct MidiFile* pMidiFile) {
  if(!pMidiFile)
    return EMIDI_INVALID_HANDLE;

  fclose(pMidiFile->p);

  return EMIDI_OK;
}

