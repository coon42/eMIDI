#ifndef _MIDI_PORT
#define _MIDI_PORT

#include "emiditypes.h"

typedef struct MidiPortInfo {
  int id;
  char pName[64];
} MidiPortInfo;

Error eMidi_enumPorts(MidiPortInfo* pPortInfo);

#endif // !_MIDI_PORT
