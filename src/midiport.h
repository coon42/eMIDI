#ifndef _MIDI_PORT
#define _MIDI_PORT

#include <windows.h> // TODO: remove windows dependency!!!
#include "emiditypes.h"

typedef struct MidiPortInfo {
  int id;
  char pName[64];
} MidiPortInfo;

typedef void(*OnMidiMsgCallback_t)(void* pArgs, uint8_t status, uint8_t param1, uint8_t param2);

typedef struct MidiInPort {
  HMIDIIN hMidiIn; // TODO: remove windows dependency!!!
  MidiPortInfo info;
  OnMidiMsgCallback_t callback;
  void* pCallbackArgs;
} MidiInPort;

Error eMidi_enumInPorts(uint32_t index, MidiPortInfo* pPortInfo);
Error eMidi_openInPort(MidiInPort* pPort, uint32_t index, OnMidiMsgCallback_t callback, void* callbackArgs);
Error eMidi_closeInPort(MidiInPort* pPort);
Error eMidi_enumOutPorts(uint32_t index, MidiPortInfo* pPortInfo);

#endif // !_MIDI_PORT
