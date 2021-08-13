#include <string.h>
#include <stdlib.h>
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
  return prvReadVoid(p, pOut, sizeof(uint16_t), pNumBytesRead);
}

static Error prvReadDword(FILE* p, uint32_t* pOut, int* pNumBytesRead) {
  return prvReadVoid(p, pOut, sizeof(uint32_t), pNumBytesRead);
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

static Error prvWriteVoid(FILE* p, const void* pData, int len) {
  int n = eMidi_fwrite(pData, 1, len, p);

  if(n < len)
    return EMIDI_CANNOT_WRITE_TO_FILE;

  return EMIDI_OK;
}

static Error prvWriteByte(FILE* p, uint8_t data) {
  return prvWriteVoid(p, &data, sizeof(uint8_t));
}

static Error prvWriteWord(FILE* p, uint16_t data) {
  return prvWriteVoid(p, &data, sizeof(uint16_t));
}

static Error prvWriteDword(FILE* p, uint32_t data) {
  return prvWriteVoid(p, &data, sizeof(uint32_t));
}

static Error prvWriteVarLen(FILE* p, uint32_t value) {
  uint32_t buffer = value & 0x7f;

  while ((value >>= 7) > 0) {
    buffer <<= 8;
    buffer |= 0x80;
    buffer += (value & 0x7f);
  }

  Error error;

  while (true) {
    uint8_t c = (uint8_t)buffer;

    if(error = prvWriteByte(p, c))
      return error;

    if (buffer & 0x80)
      buffer >>= 8;
    else
      break;
  }

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
//--------------------------------------------------------------------------------------------------

Error eMidi_open(MidiFile* pMidiFile, const char* pFileName) {
  if(!pMidiFile)
    return EMIDI_INVALID_HANDLE;

  FILE* p = eMidi_fopen(pFileName, "rb");

  if(!p)
    return EMIDI_CANNOT_OPEN_FILE;

  pMidiFile->mode = MIDI_FILE_MODE_READ;

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

  if(error = prvReadVoid(p, &header, chunkInfo.length, &numBytesRead))
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

  if(!pMidiFile)
    return EMIDI_INVALID_HANDLE;

  if(pMidiFile->mode != MIDI_FILE_MODE_READ)
    return EMIDI_INVALID_FILE_MODE;

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
//--------------------------------------------------------------------------------------------------

Error eMidi_create(MidiFile* pMidiFile, const char* pFileName) {
  memset(pMidiFile, 0, sizeof(MidiFile));
  pMidiFile->mode = MIDI_FILE_MODE_CREATE;
  pMidiFile->p = eMidi_fopen(pFileName, "wb");

  if (!pMidiFile->p)
    return EMIDI_CANNOT_OPEN_FILE;

  return EMIDI_OK;
}

static Error writeEvent(MidiFile* pMidiFile, const MidiEvent* pEvent) {
  MidiEventList* pList = (MidiEventList*)malloc(sizeof(MidiEventList));
  pList->event = *pEvent;
  pList->pNext = NULL;

  if(pMidiFile->pEventList) {
    MidiEventList* pLast;

    for(pLast = pMidiFile->pEventList; pLast->pNext; pLast = pLast->pNext)
      ; // Seek to end of list. TODO: save pointer to last entry somewhere instead of seeking

    pLast->pNext = pList;
  }
  else
    pMidiFile->pEventList = pList;

  return EMIDI_OK;
}

Error eMidi_writeNoteOffEvent(MidiFile* pMidiFile, uint32_t deltaTime, uint8_t channel, uint8_t note, uint8_t velocity) {
  MidiEvent e = { 0 };
  e.deltaTime = deltaTime;
  e.eventId = MIDI_EVENT_NOTE_OFF | (channel & 0x0F);
  e.params.msg.noteOn.note = note;
  e.params.msg.noteOn.velocity = velocity;

  return writeEvent(pMidiFile, &e);
}

Error eMidi_writeNoteOnEvent(MidiFile* pMidiFile, uint32_t deltaTime, uint8_t channel, uint8_t note, uint8_t velocity) {
  MidiEvent e = { 0 };
  e.deltaTime = deltaTime;
  e.eventId = MIDI_EVENT_NOTE_ON | (channel & 0x0F);
  e.params.msg.noteOn.note = note;
  e.params.msg.noteOn.velocity = velocity;

  return writeEvent(pMidiFile, &e);
}

Error eMidi_writeControlChangeEvent(MidiFile* pMidiFile, uint32_t deltaTime, uint8_t channel, uint8_t control, uint8_t value) {
  MidiEvent e = { 0 };
  e.deltaTime = deltaTime;
  e.eventId = MIDI_EVENT_CONTROL_CHANGE | (channel & 0x0F);
  e.params.msg.controlChange.control = control;
  e.params.msg.controlChange.value = value;

  return writeEvent(pMidiFile, &e);
}

Error eMidi_writeProgramChangeEvent(MidiFile* pMidiFile, uint32_t deltaTime, uint8_t channel, uint8_t programNumber) {
  MidiEvent e = { 0 };
  e.deltaTime = deltaTime;
  e.eventId = MIDI_EVENT_PROGRAM_CHANGE | (channel & 0x0F);
  e.params.msg.programChange.programNumber = programNumber;

  return writeEvent(pMidiFile, &e);
}

Error eMidi_writeEndOfTrackMetaEvent(MidiFile* pMidiFile, uint32_t deltaTime) {
  MidiEvent e = { 0 };
  e.deltaTime = deltaTime;
  e.eventId = MIDI_EVENT_META;
  e.metaEventId = MIDI_END_OF_TRACK;
  e.metaEventLen = 0;

  return writeEvent(pMidiFile, &e);
}

Error eMidi_writeSetTempoMetaEvent(MidiFile* pMidiFile, uint32_t deltaTime, uint32_t bpm) {
  static const uint32_t c = 60000000;

  MidiEvent e = { 0 };
  e.deltaTime = deltaTime;
  e.eventId = MIDI_EVENT_META;
  e.metaEventId = MIDI_SET_TEMPO;
  e.metaEventLen = 3;
  e.params.msg.meta.setTempo.usPerQuarterNote = c / bpm;

  return writeEvent(pMidiFile, &e);
}

static Error prvWriteTrackHeader(MidiFile* pMidiFile) {
  int32_t originalPos = eMidi_ftell(pMidiFile->p);

  Error error;

  if (error = eMidi_fseek(pMidiFile->p, 0, SEEK_END))
    return error;

  int32_t fileSize = eMidi_ftell(pMidiFile->p);

  if (error = eMidi_fseek(pMidiFile->p, sizeof(MidiChunkInfo) + sizeof(MidiHeader), SEEK_SET))
    return error;

  int32_t trackSize = fileSize - (2 * sizeof(MidiChunkInfo) + sizeof(MidiHeader));

  MidiChunkInfo chunkInfo;
  memcpy(chunkInfo.pChunk, "MTrk", 4);
  chunkInfo.length = BSWAP_32(trackSize);

  if(error = prvWriteVoid(pMidiFile->p, &chunkInfo, sizeof(MidiChunkInfo)))
    return error;

  if (error = eMidi_fseek(pMidiFile->p, originalPos, SEEK_SET))
    return error;

  return EMIDI_OK;
}

Error eMidi_save(MidiFile* pMidiFile) {
  if(!pMidiFile)
    return EMIDI_INVALID_HANDLE;

  if(pMidiFile->mode != MIDI_FILE_MODE_CREATE)
    return EMIDI_INVALID_FILE_MODE;

  if (!pMidiFile->p)
    return EMIDI_FILE_NOT_OPENED;

  FILE* p = pMidiFile->p;

  Error error;

  MidiChunkInfo chunkInfo;
  memcpy(chunkInfo.pChunk, "MThd", 4);
  chunkInfo.length = BSWAP_32(6);

  if(error = prvWriteVoid(p, &chunkInfo, sizeof(MidiChunkInfo)))
    return error;

  MidiHeader header;
  header.format       = BSWAP_16(0);
  header.ntrks        = BSWAP_16(1);
  header.division.raw = BSWAP_16(960); // TODO: Make division configurable by user

  if(error = prvWriteVoid(p, &header, sizeof(MidiHeader)))
    return error;

  memcpy(chunkInfo.pChunk, "MTrk", 4);
  chunkInfo.length = 0;

  if(error = prvWriteVoid(p, &chunkInfo, sizeof(MidiChunkInfo)))
    return error;

  for(const MidiEventList* pList = pMidiFile->pEventList; pList; pList = pList->pNext) {
    MidiEvent e = pList->event;

    if(error = prvWriteVarLen(pMidiFile->p, e.deltaTime))
      return error;

    if(error = prvWriteByte(pMidiFile->p, e.eventId))
      return error;

    if(e.eventId != MIDI_EVENT_META) {
      uint8_t paramLen = 0;

      switch(e.eventId & 0xF0) {
        case MIDI_EVENT_NOTE_OFF:          paramLen = 2; break;
        case MIDI_EVENT_NOTE_ON:           paramLen = 2; break;
        case MIDI_EVENT_POLY_KEY_PRESSURE: paramLen = 2; break;
        case MIDI_EVENT_CONTROL_CHANGE:    paramLen = 2; break;
        case MIDI_EVENT_PROGRAM_CHANGE:    paramLen = 1; break;
        case MIDI_EVENT_CHANNEL_PRESSURE:  paramLen = 1; break;
        case MIDI_EVENT_PITCH_BEND:        paramLen = 2; break;
      }

      if(error = prvWriteVoid(pMidiFile->p, &e.params.msg, paramLen))
        return error;
    }
    else {
      if(error = prvWriteByte(pMidiFile->p, e.metaEventId))
        return error;

      if(error = prvWriteByte(pMidiFile->p, e.metaEventLen))
        return error;

      switch(e.metaEventId) {
        case MIDI_SET_TEMPO:
          e.params.msg.meta.setTempo.usPerQuarterNote = BSWAP_32(e.params.msg.meta.setTempo.usPerQuarterNote) >> 8;
          break;
      }

      if(e.metaEventLen) {
        if(error = prvWriteVoid(pMidiFile->p, &e.params.msg, e.metaEventLen))
          return error;
      }
    }
  }

  if(error = prvWriteTrackHeader(pMidiFile))
    return error;

  return EMIDI_OK;
}

//--------------------------------------------------------------------------------------------------
// Common
//-------------------------------------------------------------------------------------------------

static Error closeRead(MidiFile* pMidiFile) {
  if (!pMidiFile)
    return EMIDI_INVALID_HANDLE;

  if (!pMidiFile->p)
    return EMIDI_FILE_NOT_OPENED;

  return EMIDI_OK;
}

static Error closeCreate(MidiFile* pMidiFile) {
  if (!pMidiFile)
    return EMIDI_INVALID_HANDLE;

  if (!pMidiFile->p)
    return EMIDI_FILE_NOT_OPENED;
    
  MidiEventList* p = pMidiFile->pEventList;

  while(p) {
    MidiEventList* pNext = p->pNext;
    free(p);

    p = pNext;
  }     

  return EMIDI_OK;
}

Error eMidi_close(MidiFile* pMidiFile) {
  Error error;

  switch(pMidiFile->mode) {
    case MIDI_FILE_MODE_READ: {
      if (error = closeRead(pMidiFile))
        return error;

      break;
    }

    case MIDI_FILE_MODE_CREATE: {
      if (error = closeCreate(pMidiFile))
        return error;

      break;
    }

    default:
      return EMIDI_INVALID_FILE_MODE;
  }

  eMidi_fclose(pMidiFile->p);

  return EMIDI_OK;
}
