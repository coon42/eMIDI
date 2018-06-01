#ifndef _MIDI_PORT
#define _MIDI_PORT

#include "emiditypes.h"

typedef struct MidiPortInfo {
  int id;
  char pName[64];
} MidiPortInfo;

Error eMidi_enumInPorts(uint32_t index, MidiPortInfo* pPortInfo);
Error eMidi_enumOutPorts(uint32_t index, MidiPortInfo* pPortInfo);

#endif // !_MIDI_PORT
