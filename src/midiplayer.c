#define _DEFAULT_SOURCE
#include <unistd.h>

#include <time.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "midifile.h"
#include "helpers.h"
#include "midiplayer.h"

typedef struct MidiPlayer {
  MidiFile midi;
  MidiEvent event;
  uint32_t uspqn;
  uint32_t lastReloadTimeUs;
  UserEventCallback_t pUserEventCallback;
  void* pContext;
} MidiPlayer;

static void listDevices() {
  const char* pDevice = "/dev/sequencer";
  int fd = open(pDevice, O_WRONLY, 0);

  if (fd < 0) {
    printf("Error: cannot open %s\n", pDevice);
    exit(1);
  }

  int midiCount;
  int status = ioctl(fd, SNDCTL_SEQ_NRMIDIS, &midiCount);

  int synthCount;
  status = ioctl(fd, SNDCTL_SEQ_NRSYNTHS, &synthCount);

  struct midi_info midiinfo;

  for (int i = 0; i < midiCount; i++) {
    midiinfo.device = i;
    status = ioctl(fd, SNDCTL_MIDI_INFO, &midiinfo);
    printf("MIDI Port %d: %s\n", i, midiinfo.name);
  }

  struct synth_info synthinfo;

  for (int i=0; i < synthCount; i++) {
    synthinfo.device = i;
    status = ioctl(fd, SNDCTL_SYNTH_INFO, &synthinfo);
    printf("Synth Port %d: %s\n", i, synthinfo.name);
  }
}

static int timeUs() {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);

  return t.tv_sec * 1000000 + t.tv_nsec / 1000;
}

static void dispatchEvent(MidiPlayer* pPlayer) {
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

static Error midiPlayerOpen(MidiPlayer* pPlayer, const char* pFileName,
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

static Error midiPlayerTick(MidiPlayer* pPlayer) {
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

static Error play(MidiPlayer* pPlayer) {
  while(midiPlayerTick(pPlayer) == EMIDI_OK);
}

typedef struct MyContext {
  int32_t fd;
  uint8_t devnum;
} MyContext;

static void userEventCallback(MidiEvent* pEvent, void* pContext) {
  MyContext* pCtx = (MyContext*)pContext;

  int numParamBytes = 0;

  switch(pEvent->eventId & 0xF0) {
    case MIDI_EVENT_NOTE_OFF:          numParamBytes = 2; break;
    case MIDI_EVENT_NOTE_ON:           numParamBytes = 2; break;
//    case MIDI_EVENT_POLY_KEY_PRESSURE: numParamBytes = 2; break;
//    case MIDI_EVENT_CONTROL_CHANGE:    numParamBytes = 2; break;
    case MIDI_EVENT_PROGRAM_CHANGE:    numParamBytes = 1; break;
//    case MIDI_EVENT_CHANNEL_PRESSURE:  numParamBytes = 1; break;
    case MIDI_EVENT_PITCH_BEND:        numParamBytes = 2; break;

    default:
      return;
  }

  uint8_t packet[4] = { SEQ_MIDIPUTC, 0, pCtx->devnum, 0 };

  packet[1] = pEvent->eventId;
  write(pCtx->fd, packet, sizeof(packet));

  for(int i = 0; i < numParamBytes; ++i) {
    packet[1] = pEvent->params.pRaw[i];
    write(pCtx->fd, packet, sizeof(packet));
  }

  eMidi_printMidiEvent(pEvent);
}

int main(int argc, char* pArgv[]) {
  const char* pDevice = "/dev/sequencer";

  MyContext ctx;
  ctx.fd = open(pDevice, O_WRONLY, 0);
  ctx.devnum = 1;

  if(ctx.fd < 0) {
    printf("Error: cannot open %s\n", pDevice);
    exit(1);
  }

  if(argc < 2) {
    printf("Usage: player <midi file>");

    return 1;
  }

  const char* pMidiFileName = pArgv[1];

  Error error;
  MidiPlayer player;

  if(error = midiPlayerOpen(&player, pMidiFileName, userEventCallback, &ctx)) {
    printf("Cannot open file: '%s'\n", pMidiFileName);
    eMidi_printError(error);

    return 2;
  }

  printf("Midi file '%s' opened successfully!\n", pMidiFileName);

  if(error = play(&player))
    return error;

  eMidi_close(&player.midi);
  close(ctx.fd);

  return 0;
}

