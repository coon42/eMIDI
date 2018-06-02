#ifndef _MIDI_PORT
#define _MIDI_PORT

#include "emiditypes.h"

typedef struct MidiPortInfo {
  int id;
  char pName[64];
} MidiPortInfo;

typedef struct MidiInPort {
  MidiPortInfo info;
} MidiInPort;

Error eMidi_enumInPorts(uint32_t index, MidiPortInfo* pPortInfo);
Error eMidi_enumOutPorts(uint32_t index, MidiPortInfo* pPortInfo);
Error eMidi_openInPort(MidiInPort* pPort, uint32_t index);

#endif // !_MIDI_PORT
