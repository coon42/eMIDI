#include <string.h>
#include "midifile.h"
#include "mybyteswap.h"
#include "hal/emidi_hal.h"

static Error prvSkipBytes(FILE* p, int len) {
  if(eMidi_fseek(p, len, SEEK_CUR))
    return EMIDI_UNEXPECTED_END_OF_FILE;

  return EMIDI_OK;
}

static Error prvReadVoid(FILE* p, void* pOut, int len, int* pNumBytesRead) {
  int n = eMidi_fread(pOut, 1, len, p);

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

static Error prvReadVarLen(FILE* p, uint32_t* pLen) {
  Error error;
  uint32_t value;
  uint8_t c;

  if(error = prvReadByte(p, &c, NULL))
    return error;

  value = c;

  if(c & 0x80) {
    value &= 0x7f;

    do {
      if(error = prvReadByte(p, &c, NULL))
        return error;

      value = (value << 7) + (c & 0x7f);
    } while (c & 0x80);
  }

  *pLen = value;

  return EMIDI_OK;
}

static uint32_t prvGetFileSize(FILE* p) {
  eMidi_fseek(p, 0, SEEK_END);
  uint32_t fileSize = eMidi_ftell(p);
  eMidi_fseek(p, 0, SEEK_SET);

  return fileSize;
}

//--------------------------------------------------------------------------------------------------
// Read-API
//-------------------------------------------------------------------------------------------------

Error eMidi_open(MidiFile* pMidiFile, const char* pFileName) {
  if(!pMidiFile)
    return EMIDI_INVALID_HANDLE;

  FILE* p = eMidi_fopen(pFileName, "rb");

  if(!p)
    return EMIDI_CANNOT_OPEN_FILE;

  uint32_t fileSize = prvGetFileSize(p);

  MidiChunkInfo chunkInfo;
  Error error;
  int32_t numBytesRead;

  if(error = prvReadVoid(p, &chunkInfo, sizeof(MidiChunkInfo), &numBytesRead))
    return error;

  if(memcmp(chunkInfo.pChunk, "MThd", 4) != 0)
    return EMIDI_INVALID_MIDI_FILE;

  chunkInfo.length = BSWAP_32(chunkInfo.length);

  if(chunkInfo.length != 6) // Might change in a future MIDI standard
    return EMIDI_INVALID_MIDI_FILE;

  MidiHeader header;

  if(error = error = prvReadVoid(p, &header, chunkInfo.length, &numBytesRead))
    return error;

  header.format       = BSWAP_16(header.format);
  header.ntrks        = BSWAP_16(header.ntrks);
  header.division.raw = BSWAP_16(header.division.raw);

  if(header.format > 2)
    return EMIDI_INVALID_MIDI_FILE;

  if(header.format == 1)
    return EMIDI_FORMAT_1_NOT_SUPPORTED;

  if(header.format == 2)
    return EMIDI_FORMAT_2_NOT_SUPPORTED;

  if(header.division.format != DIVISION_TPQN)
    return EMIDI_DIVISION_FORMAT_NOT_SUPPORTED;

  if(header.format == 0 && header.ntrks != 1)
    return EMIDI_INVALID_MIDI_FILE;

  if(error = prvReadVoid(p, &chunkInfo, sizeof(MidiChunkInfo), &numBytesRead))
    return error;

  if(memcmp(chunkInfo.pChunk, "MTrk", 4) != 0)
    return EMIDI_INVALID_MIDI_FILE;

  pMidiFile->p = p;
  pMidiFile->size = fileSize;
  pMidiFile->header = header;
  pMidiFile->track.startPos = eMidi_ftell(p);
  pMidiFile->track.pos = eMidi_ftell(p);
  pMidiFile->track.len = chunkInfo.length;
  pMidiFile->prevEventId = 0;

  return EMIDI_OK;
}

Error eMidi_readEvent(MidiFile* pMidiFile, MidiEvent* pEvent) {
  Error error;

  if(error = prvReadVarLen(pMidiFile->p, &pEvent->deltaTime))
    return error;

  if(error = prvReadByte(pMidiFile->p, &pEvent->eventId, NULL))
    return error;

  if(pEvent->eventId & 0x80) {
    pEvent->isRunningStatus = false;
    pMidiFile->prevEventId = pEvent->eventId;
  }
  else {
    pEvent->isRunningStatus = true;
    pEvent->eventId = pMidiFile->prevEventId;

    // TODO: do not read first data byte again. Skip second read instead:
    eMidi_fseek(pMidiFile->p, -1, SEEK_CUR);
  }

  // First check for channel messages:

  switch(pEvent->eventId & 0xF0) {
    case MIDI_EVENT_NOTE_OFF:
      prvReadByte(pMidiFile->p, &pEvent->params.pRaw[0], NULL);
      prvReadByte(pMidiFile->p, &pEvent->params.pRaw[1], NULL);
      pEvent->params.msg.noteOff.note     = pEvent->params.pRaw[0];
      pEvent->params.msg.noteOff.velocity = pEvent->params.pRaw[1];
      return EMIDI_OK; // TODO: DRY return code

    case MIDI_EVENT_NOTE_ON:
      prvReadByte(pMidiFile->p, &pEvent->params.pRaw[0], NULL);
      prvReadByte(pMidiFile->p, &pEvent->params.pRaw[1], NULL);
      pEvent->params.msg.noteOn.note     = pEvent->params.pRaw[0];
      pEvent->params.msg.noteOn.velocity = pEvent->params.pRaw[1];
      return EMIDI_OK; // TODO: DRY return code

    case MIDI_EVENT_POLY_KEY_PRESSURE:
      prvReadByte(pMidiFile->p, &pEvent->params.pRaw[0], NULL);
      prvReadByte(pMidiFile->p, &pEvent->params.pRaw[1], NULL);
      // TODO: implement
      return EMIDI_OK; // TODO: DRY return code

    case MIDI_EVENT_CONTROL_CHANGE:
      prvReadByte(pMidiFile->p, &pEvent->params.pRaw[0], NULL);
      prvReadByte(pMidiFile->p, &pEvent->params.pRaw[1], NULL);
      pEvent->params.msg.controlChange.control = pEvent->params.pRaw[0];
      pEvent->params.msg.controlChange.value   = pEvent->params.pRaw[1];
      return EMIDI_OK; // TODO: DRY return code

    case MIDI_EVENT_PROGRAM_CHANGE:
      prvReadByte(pMidiFile->p, &pEvent->params.pRaw[0], NULL);
      pEvent->params.msg.programChange.programNumber = pEvent->params.pRaw[0];
      return EMIDI_OK; // TODO: DRY return code

    case MIDI_EVENT_CHANNEL_PRESSURE:
      prvReadByte(pMidiFile->p, &pEvent->params.pRaw[0], NULL);
      // TODO: implement
      return EMIDI_OK; // TODO: DRY return code

    case MIDI_EVENT_PITCH_BEND: {
      prvReadByte(pMidiFile->p, &pEvent->params.pRaw[0], NULL);
      prvReadByte(pMidiFile->p, &pEvent->params.pRaw[1], NULL);

      uint16_t pitchBendValue = pEvent->params.pRaw[0];
      pitchBendValue |= (pEvent->params.pRaw[1] << 7);

      pEvent->params.msg.pitchBend.value = pitchBendValue;
      return EMIDI_OK; // TODO: DRY return code
    }
  }

  // Now Check for System Messages:

  switch(pEvent->eventId) {
    case MIDI_EVENT_SYSTEM_EXCLUSIVE: {
      // Ignore all bytes until EOX byte (0xF7):

      uint8_t dataByte;

      do {
       if(error = prvReadByte(pMidiFile->p, &dataByte, NULL))
        return error;

      } while(dataByte != MIDI_EVENT_END_OF_EXCLUSIVE);

      break;
    }

    case MIDI_EVENT_META: {
      if(error = prvReadByte(pMidiFile->p, &pEvent->metaEventId, NULL))
        return error;

      if(error = prvReadByte(pMidiFile->p, &pEvent->metaEventLen, NULL))
        return error;

      switch(pEvent->metaEventId) {
        case MIDI_META_MIDI_PORT: {
          uint8_t port;

          if(error = prvReadVoid(pMidiFile->p, &port, 1, NULL))
            return error;

          pEvent->params.msg.meta.midiPort.port = port;

          break;
        }

        case MIDI_SET_TEMPO: {
          uint32_t uspqn;

          if(error = prvReadVoid(pMidiFile->p, &uspqn, 3, NULL))
            return error;

          pEvent->params.msg.meta.setTempo.usPerQuarterNote = BSWAP_32(uspqn) >> 8;

          break;
        }

        default:
          // Ignore params of unimplemented meta events:
          if(error = prvSkipBytes(pMidiFile->p, pEvent->metaEventLen))
            return error;
      }

      break;
    }

    // TODO: check for additional events which can occur in MIDI files:
    default:
      return EMIDI_SYNTAX_ERROR;
  }

  return EMIDI_OK;
}


//--------------------------------------------------------------------------------------------------
// Write-API
//-------------------------------------------------------------------------------------------------

Error eMidi_create(MidiFile* pMidiFile, const char* pFileName) {
  return EMIDI_FUNCTION_NOT_IMPLEMENTED;
}

Error eMidi_save(MidiFile* pMidiFile) {
  return EMIDI_FUNCTION_NOT_IMPLEMENTED;
}


//--------------------------------------------------------------------------------------------------
// Common
//-------------------------------------------------------------------------------------------------

Error eMidi_close(MidiFile* pMidiFile) {
  if(!pMidiFile)
    return EMIDI_INVALID_HANDLE;

  if (pMidiFile->p)
    eMidi_fclose(pMidiFile->p);

  return EMIDI_OK;
}

