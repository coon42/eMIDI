#include "midiport.h"

#include <windows.h> // TODO: move to windows HAL implementation
#include <string.h>

typedef enum PortType {
  IN_PORT,
  OUT_PORT
} PortType;

static Error getPortName(char* pPortName, int nCount, PortType portType, int index) {
  MIDIINCAPS inCaps;
  MIDIOUTCAPS outCaps;
  const wchar_t* pName;

  int numDevs = portType == IN_PORT ? midiInGetNumDevs() : midiOutGetNumDevs();

  if (index >= numDevs)
    return EMIDI_OK_END_OF_PORTS;

  if (portType == IN_PORT) {
    midiInGetDevCaps(index, &inCaps, sizeof(MIDIINCAPS));
    pName = inCaps.szPname;
  }
  else {
    midiOutGetDevCaps(index, &outCaps, sizeof(MIDIOUTCAPS));
    pName = outCaps.szPname;
  }

#if defined(UNICODE) || defined( _UNICODE )
  memset(pPortName, 0, nCount);

  int length = WideCharToMultiByte(CP_UTF8, 0, pName, -1, NULL, 0, NULL, NULL) - 1;
  length = length < nCount ? length : nCount;
  WideCharToMultiByte(CP_UTF8, 0, pName, (int)(wcslen(pName)), pPortName, length, NULL, NULL);
#else
  strncpy(pPortName, pName, nCount);
#endif

  return EMIDI_OK;
}

static Error prvEnumPorts(uint32_t index, MidiPortInfo* pPortInfo, PortType portType) {
  const uint32_t numDevs = portType == IN_PORT ? midiInGetNumDevs() : midiOutGetNumDevs();

  if (index >= numDevs)
    return EMIDI_OK_END_OF_PORTS;

  pPortInfo->id = index;
  getPortName(pPortInfo->pName, sizeof(pPortInfo->pName), portType, index);

  return EMIDI_OK;
}

typedef struct MM_MidiMsg_t {
  uint8_t
  status,
  param1,
  param2;
} MM_MidiMsg_t;

static void CALLBACK midiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1,
    DWORD_PTR dwParam2) {

  MidiInPort* pPort = (MidiInPort*)dwInstance;

  // TODO: sysex messages are not handled properly yet!

  switch (wMsg) {    
    case MIM_DATA: 
      if (pPort->callback) {         
        const MM_MidiMsg_t* pMidiMsg = (const MM_MidiMsg_t*)&dwParam1;
        uint32_t deltaTimeMs = (uint32_t)dwParam2;

        pPort->callback(pPort->pCallbackArgs, deltaTimeMs, pMidiMsg->status, pMidiMsg->param1, pMidiMsg->param2);
        break;
      }
  }
}

Error eMidi_enumInPorts(uint32_t index, MidiPortInfo* pPortInfo) {
  return prvEnumPorts(index, pPortInfo, IN_PORT);
}

Error eMidi_openInPort(MidiInPort* pPort, uint32_t index, OnMidiMsgCallback_t callback, void* pCallbackArgs) {
  const uint32_t numDevs = midiInGetNumDevs();

  if (index < 0 || index > numDevs - 1)
    return EMIDI_INVALID_PORT_INDEX;

  pPort->info.id = index;
  getPortName(pPort->info.pName, sizeof(pPort->info.pName), IN_PORT, index);  
  pPort->callback = callback;
  pPort->pCallbackArgs = pCallbackArgs;

  MMRESULT res;
  
  res = midiInOpen(&pPort->hMidiIn, index, (DWORD_PTR)midiInProc, (DWORD_PTR)pPort, CALLBACK_FUNCTION);
  res = midiInStart(pPort->hMidiIn);

  return EMIDI_OK;
}

Error eMidi_closeInPort(MidiInPort* pPort) {
  MMRESULT res;
  
  if (res = midiInClose(pPort->hMidiIn)) {
    // TODO: return error
  }
        
  return EMIDI_OK;
}

Error eMidi_enumOutPorts(uint32_t index, MidiPortInfo* pPortInfo) {
  return prvEnumPorts(index, pPortInfo, OUT_PORT);
}
