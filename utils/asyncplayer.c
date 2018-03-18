#define _DEFAULT_SOURCE
#include <unistd.h>

#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "midifile.h"
#include "helpers.h"

static void listDevices() {
  int status;
  int midiCount;
  const char* pDevice = "/dev/sequencer";

  int seqfd = open(pDevice, O_WRONLY, 0);
  if (seqfd < 0) {
    printf("Error: cannot open %s\n", pDevice);
    exit(1);
  }

  status = ioctl(seqfd, SNDCTL_SEQ_NRMIDIS, &midiCount);

  int synthCount;
  status = ioctl(seqfd, SNDCTL_SEQ_NRSYNTHS, &synthCount);

  struct midi_info midiinfo;
  for (int i = 0; i < midiCount; i++) {
    midiinfo.device = i;
    status = ioctl(seqfd, SNDCTL_MIDI_INFO, &midiinfo);
    printf("MIDI Port %d: %s\n", i, midiinfo.name);
  }

  struct synth_info synthinfo;
  for (int i=0; i < synthCount; i++) {
    synthinfo.device = i;
    status = ioctl(seqfd, SNDCTL_SYNTH_INFO, &synthinfo);
    printf("Synth Port %d: %s\n", i, synthinfo.name);
  }
}

static void sendMidiMsg(int fd, int devnum, MidiEvent e) {
  uint8_t packet[4] = {SEQ_MIDIPUTC, 0, devnum, 0};

  switch(e.eventId & 0xF0) {
    case MIDI_EVENT_NOTE_OFF:
      packet[1] = e.eventId;
      write(fd, packet, sizeof(packet));

      packet[1] = e.params.msg.noteOff.note;
      write(fd, packet, sizeof(packet));

      packet[1] = e.params.msg.noteOff.velocity;
      write(fd, packet, sizeof(packet));
      break;

    case MIDI_EVENT_NOTE_ON:
      packet[1] = e.eventId;
      write(fd, packet, sizeof(packet));

      packet[1] = e.params.msg.noteOn.note;
      write(fd, packet, sizeof(packet));

      packet[1] = e.params.msg.noteOn.velocity;
      write(fd, packet, sizeof(packet));
      break;

    case MIDI_EVENT_PROGRAM_CHANGE:
      packet[1] = e.eventId;
      write(fd, packet, sizeof(packet));

      packet[1] = e.params.msg.programChange.programNumber;
      write(fd, packet, sizeof(packet));
      break;

    default:
      return;
  }
}

typedef struct MidiPlayer {
  MidiFile midi;
  uint32_t uspqn;
  uint32_t nextEventTimeUs;

  // TODO: move to callback context::
  int32_t fd;
  uint8_t devnum;
} MidiPlayer;

static void midiPlayerInit(MidiPlayer* pPlayer) {
  static const uint32_t c = 60000000;
  static const uint32_t defaultBpm = 120;

  pPlayer->uspqn = c / defaultBpm;
  pPlayer->nextEventTimeUs = 0;
}

static int midiPlayerTick(MidiPlayer* pPlayer) {
  Error error;
  MidiEvent e;

  if(error = eMidi_readEvent(&pPlayer->midi, &e)) {
    printf("Error on reading event: [0x%02X] (Error %d: %s)\n",e.eventId, error,
        eMidi_errorToStr(error));

    return 3;
  }

  if(e.eventId == MIDI_EVENT_META) {
    if(e.metaEventId == MIDI_SET_TEMPO)
      pPlayer->uspqn = e.params.msg.meta.setTempo.usPerQuarterNote;
  }

  uint32_t TQPN = pPlayer->midi.header.division.tqpn.TQPN;
  uint32_t usToWait = (e.deltaTime * pPlayer->uspqn) / TQPN;

  usleep(usToWait);

  eMidi_printMidiEvent(&e);
  sendMidiMsg(pPlayer->fd, pPlayer->devnum, e);

  return (!(e.eventId == MIDI_EVENT_META && e.metaEventId == MIDI_END_OF_TRACK));
}

static Error play(MidiPlayer* pPlayer) {
  while(midiPlayerTick(pPlayer));
}

int main(int argc, char* pArgv[]) {
  const char* pDevice = "/dev/sequencer";
  uint8_t devnum = 1;

  // open the OSS device for writing:
  int fd = open(pDevice, O_WRONLY, 0);
  if(fd < 0) {
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
  midiPlayerInit(&player);

  player.fd = fd;
  player.devnum = devnum;

  if(error = eMidi_open(&player.midi, pMidiFileName)) {
    printf("Cannot open file: '%s'\n", pMidiFileName);
    eMidi_printError(error);

    return 2;
  }

  printf("Midi file '%s' opened successfully!\n", pMidiFileName);

  if(error = play(&player))
    return error;

  eMidi_close(&player.midi);
  close(fd);

  return 0;
}

