#include "midiport.h"

#include <windows.h> // TODO: move to windows HAL implementation
#include <string.h>

typedef enum PortType {
  IN_PORT,
  OUT_PORT
} PortType;

static Error prvEnumPorts(uint32_t index, MidiPortInfo* pPortInfo, PortType portType) {
  int numDevs = portType == IN_PORT ? midiInGetNumDevs() : midiOutGetNumDevs();

  if (index >= numDevs)
    return EMIDI_OK_END_OF_PORTS;

  MIDIINCAPS inCaps;
  MIDIOUTCAPS outCaps;
  const wchar_t* pName;

  if (portType == IN_PORT) {
    midiInGetDevCaps(index, &inCaps, sizeof(MIDIINCAPS));
    pName = inCaps.szPname;
  }
  else {
    midiOutGetDevCaps(index, &outCaps, sizeof(MIDIOUTCAPS));
    pName = outCaps.szPname;
  }

  pPortInfo->id = index;

#if defined(UNICODE) || defined( _UNICODE )
  memset(pPortInfo->pName, 0, sizeof(pPortInfo->pName));

  int length = WideCharToMultiByte(CP_UTF8, 0, pName, -1, NULL, 0, NULL, NULL) - 1;
  WideCharToMultiByte(CP_UTF8, 0, pName, (int)(wcslen(pName)), pPortInfo->pName, length, NULL, NULL);

#else
  strncpy(pPortInfo->pName, pName, sizeof(pPortInfo->pName));
#endif

  return EMIDI_OK;
}

static void CALLBACK midiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1,
    DWORD_PTR dwParam2) {

  MidiInPort* pPort = (MidiInPort*)dwInstance;

  if(pPort->callback)
    pPort->callback(pPort->pCallbackArgs);
}

Error eMidi_enumInPorts(uint32_t index, MidiPortInfo* pPortInfo) {
  return prvEnumPorts(index, pPortInfo, IN_PORT);
}

Error eMidi_openInPort(MidiInPort* pPort, uint32_t index, OnMidiMsgCallback_t callback, void* pCallbackArgs) {
  int numDevs = midiInGetNumDevs();

  if (index < 0 || index > numDevs - 1)
    return EMIDI_INVALID_PORT_INDEX;

  pPort->info.id = index;
  pPort->info.pName[0]; // TODO: set proper name
  pPort->callback = callback;
  pPort->pCallbackArgs = pCallbackArgs;

  MMRESULT res;

  HMIDIIN hMidiIn;
  res = midiInOpen(&hMidiIn, index, (DWORD_PTR)midiInProc, (DWORD_PTR)pPort, CALLBACK_FUNCTION);
  res = midiInStart(hMidiIn);

  return EMIDI_OK;
}

Error eMidi_enumOutPorts(uint32_t index, MidiPortInfo* pPortInfo) {
  return prvEnumPorts(index, pPortInfo, OUT_PORT);
}
