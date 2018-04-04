#define _DEFAULT_SOURCE
#include <unistd.h>

#include <time.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#include "midiplayer.h"

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

static Error play(MidiPlayer* pPlayer) {
  while(eMidi_playerTick(pPlayer) == EMIDI_OK);
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

  if(error = eMidi_openPlayer(&player, pMidiFileName, userEventCallback, &ctx)) {
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

