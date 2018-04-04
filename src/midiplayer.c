#define _DEFAULT_SOURCE
#include <unistd.h>
#include <time.h>

#include "midiplayer.h"

// TODO: move to HAL:
static int timeUs() {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);

  return t.tv_sec * 1000000 + t.tv_nsec / 1000;
}
// -

static void dispatchEvent(const MidiPlayer* pPlayer) {
  pPlayer->pUserEventCallback(&pPlayer->event, pPlayer->pContext);
}

static Error reload(MidiPlayer* pPlayer) {
  Error error;

  if(error = eMidi_readEvent(&pPlayer->midi, &pPlayer->event))
    return error;

  pPlayer->lastReloadTimeUs = timeUs();

  return EMIDI_OK;
}

static Error shoot(MidiPlayer* pPlayer) {
  if(pPlayer->event.eventId == MIDI_EVENT_META) {
    if(pPlayer->event.metaEventId == MIDI_SET_TEMPO)
      pPlayer->uspqn = pPlayer->event.params.msg.meta.setTempo.usPerQuarterNote;
  }

  dispatchEvent(pPlayer);

  if(pPlayer->event.eventId == MIDI_EVENT_META && pPlayer->event.metaEventId == MIDI_END_OF_TRACK)
    return EMIDI_OK_END_OF_FILE;

  return EMIDI_OK;
}

Error eMidi_openPlayer(MidiPlayer* pPlayer, const char* pFileName,
    UserEventCallback_t pUserEventCallback, void* pContext) {
  Error error;

  if(error = eMidi_open(&pPlayer->midi, pFileName))
    return error;

  if(error = reload(pPlayer))
    return error;

  static const uint32_t c = 60000000;
  static const uint32_t defaultBpm = 120;

  pPlayer->uspqn = c / defaultBpm;
  pPlayer->pUserEventCallback = pUserEventCallback;
  pPlayer->pContext = pContext;

  return EMIDI_OK;
}

Error eMidi_closePlayer(MidiPlayer* pPlayer) {
  Error error;

  if (error = eMidi_close(&pPlayer->midi))
    return error;

  return EMIDI_OK;
}

Error eMidi_playerTick(MidiPlayer* pPlayer) {
  Error error;

  uint32_t tqpn = pPlayer->midi.header.division.tqpn.TQPN;
  uint32_t usToWait = (pPlayer->event.deltaTime * pPlayer->uspqn) / tqpn;
  uint32_t usPassed = timeUs() - pPlayer->lastReloadTimeUs;

  if(usPassed < usToWait)
    return EMIDI_OK;

  if(error = shoot(pPlayer))
    return error;

  if(error = reload(pPlayer))
    return error;

  return EMIDI_OK;
}

