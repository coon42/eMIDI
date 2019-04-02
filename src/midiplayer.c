#include "midiplayer.h"
#include "hal/emidi_hal.h"

static void dispatchEvent(const MidiPlayer* pPlayer) {
  pPlayer->pUserEventCallback(&pPlayer->event, pPlayer->pContext);
}

static Error reload(MidiPlayer* pPlayer) {
  Error error;

  if (error = eMidi_readEvent(&pPlayer->midi, &pPlayer->event))
    return error;

  pPlayer->lastReloadTimeUs = eMidi_timeUs();

  return EMIDI_OK;
}

static Error shoot(MidiPlayer* pPlayer) {
  if (pPlayer->event.eventId == MIDI_EVENT_META) {
    if (pPlayer->event.metaEventId == MIDI_SET_TEMPO)
      pPlayer->uspqn = pPlayer->event.params.msg.meta.setTempo.usPerQuarterNote;
  }

  dispatchEvent(pPlayer);

  if (pPlayer->event.eventId == MIDI_EVENT_META && pPlayer->event.metaEventId == MIDI_END_OF_TRACK)
    return EMIDI_OK_END_OF_FILE;

  return EMIDI_OK;
}

Error eMidi_openPlayer(MidiPlayer* pPlayer, const char* pFileName,
    UserEventCallback_t pUserEventCallback, void* pContext) {
  Error error;

  if (error = eMidi_open(&pPlayer->midi, pFileName))
    return error;

  if (error = reload(pPlayer))
    return error;

  static const uint32_t c = 60000000;
  static const uint32_t defaultBpm = 120;

  pPlayer->uspqn = c / defaultBpm;
  pPlayer->carryUs = 0;
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
  uint32_t usPassed = eMidi_timeUs() - pPlayer->lastReloadTimeUs;
  int32_t usToWait = (pPlayer->event.deltaTime * pPlayer->uspqn) / tqpn - pPlayer->carryUs;

  if (usToWait > 0)
    if (usPassed < usToWait)
      return EMIDI_OK;

  pPlayer->carryUs = usPassed - usToWait;

  if (error = shoot(pPlayer))
    return error;

  if (error = reload(pPlayer))
    return error;

  return EMIDI_OK;
}

