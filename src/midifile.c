#include <string.h>
#include "midifile.h"
#include "byteswap.h"

static Error prvSkipBytes(FILE* p, int len) {
  int pos = ftell(p);

  if(fseek(p, len, SEEK_CUR))
    return EMIDI_UNEXPECTED_END_OF_FILE;

  return EMIDI_OK;
}

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

static uint32_t prvGetFileSize(FILE* p) {
  fseek(p, 0, SEEK_END);
  uint32_t fileSize = ftell(p);
  fseek(p, 0, SEEK_SET);

  return fileSize;
}

Error eMidi_open(MidiFile* pMidiFile, const char* pFileName) {
  if(!pMidiFile)
    return EMIDI_INVALID_HANDLE;

  FILE* p = fopen(pFileName, "rb");

  if(!p)
    return EMIDI_CANNOT_OPEN_FILE;

  uint32_t fileSize = prvGetFileSize(p);
 
  MidiChunkInfo chunkInfo;
  Error error; 
  int32_t numBytesRead;

  error = prvReadVoid(p, &chunkInfo, sizeof(MidiChunkInfo), &numBytesRead);

  if(error)
    return error;

  if(memcmp(chunkInfo.pChunk, "MThd", 4) != 0)
    return EMIDI_INVALID_MIDI_FILE;

  chunkInfo.length = __bswap_32(chunkInfo.length);

  if(chunkInfo.length != 6) // Might be change in a future MIDI standard
    return EMIDI_INVALID_MIDI_FILE;

  MidiHeader header;
  error = prvReadVoid(p, &header, chunkInfo.length, &numBytesRead);

  header.format      = __bswap_16(header.format);
  header.ntrks       = __bswap_16(header.ntrks);
  header.division    = __bswap_16(header.division);

  if(header.format > 2)
    return EMIDI_INVALID_MIDI_FILE;

  if(header.format == 1)
    return EMIDI_FORMAT_1_NOT_SUPPORTED;

  if(header.format == 2)
    return EMIDI_FORMAT_2_NOT_SUPPORTED;

  if(header.format == 0 && header.ntrks != 1)
    return EMIDI_INVALID_MIDI_FILE;    

  error = prvReadVoid(p, &chunkInfo, sizeof(MidiChunkInfo), &numBytesRead);

  if(error)
    return error;

  if(memcmp(chunkInfo.pChunk, "MTrk", 4) != 0)
    return EMIDI_INVALID_MIDI_FILE;

  pMidiFile->p = p;
  pMidiFile->size = fileSize;
  pMidiFile->header = header;
  pMidiFile->track.startPos = ftell(p);
  pMidiFile->track.pos = ftell(p);
  pMidiFile->track.len = chunkInfo.length;

  return EMIDI_OK;
}

Error eMidi_close(MidiFile* pMidiFile) {
  if(!pMidiFile)
    return EMIDI_INVALID_HANDLE;

  fclose(pMidiFile->p);

  return EMIDI_OK;
}

Error eMidi_readEvent(const MidiFile* pMidiFile, MidiEvent* pEvent) {  
  Error error;

  uint8_t len;
  uint8_t eventId;

  // TODO: read var length:
  if(error = prvReadByte(pMidiFile->p, &len, NULL))
    return error;

  pEvent->deltaTime = len;

  if(error = prvReadByte(pMidiFile->p, &pEvent->eventId, NULL))
    return error;

  if(pEvent->eventId == MIDI_EVENT_META) {
    if(error = prvReadByte(pMidiFile->p, &pEvent->metaEventId, NULL))
      return error;

    // Ignore event for now:
    if(error = prvReadByte(pMidiFile->p, &pEvent->metaEventLen, NULL))
      return error;

    if(error = prvSkipBytes(pMidiFile->p, pEvent->metaEventLen))
      return error;
  }
  else {
    int numDataBytes = 0;

    switch(pEvent->eventId) {
      case MIDI_EVENT_NOTE_ON:           numDataBytes = 2; break;
      case MIDI_EVENT_NOTE_OFF:          numDataBytes = 2; break;
      case MIDI_EVENT_POLY_KEY_PRESSURE: numDataBytes = 2; break;
      case MIDI_EVENT_CONTROL_CHANGE:    numDataBytes = 2; break;
      case MIDI_EVENT_PROGRAM_CHANGE:    numDataBytes = 1; break;
      case MIDI_EVENT_CHANNEL_PRESSURE:  numDataBytes = 1; break;
      case MIDI_EVENT_PITCH_BEND:        numDataBytes = 2; break;

      default:
        return EMIDI_FEATURE_NOT_IMPLEMENTED;
    }

    prvSkipBytes(pMidiFile->p, numDataBytes);
  }
 
  return EMIDI_OK;
}

