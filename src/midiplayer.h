#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <stdint.h>
#include "midifile.h"

typedef void(*UserEventCallback_t)(const MidiEvent* pEvent, void* pContext);

typedef struct MidiPlayer {
  MidiFile midi;
  MidiEvent event;
  uint32_t uspqn;
  uint32_t lastReloadTimeUs;
  UserEventCallback_t pUserEventCallback;
  void* pContext;
} MidiPlayer;

Error eMidi_openPlayer(MidiPlayer* pPlayer, const char* pFileName,
    UserEventCallback_t pUserEventCallback, void* pContext);

Error eMidi_playerTick(MidiPlayer* pPlayer);

#endif // !MIDIPLAYER_H

