#define _POSIX_C_SOURCE	199309L // make nanosleep available
#include <time.h>

#include "midifile.h"
#include "helpers.h"

int main(int argc, char* pArgv[]) {
  if(argc < 2) {
    printf("Usage: player <midi file>");

    return 1;
  }

  const char* pMidiFileName = pArgv[1];
  MidiFile midi;
  Error error;

  if(error = eMidi_open(&midi, pMidiFileName)) {
    printf("Cannot open file: '%s'\n", pMidiFileName);
    eMidi_printError(error);

    return 2;
  }

  printf("Midi file '%s' opened successfully!\n", pMidiFileName);

  MidiEvent e;

  do {
    if(error = eMidi_readEvent(&midi, &e)) {
      printf("Error on reading event: [0x%02X] (Error %d: %s)\n",e.eventId, error, eMidi_errorToStr(error));
      return 3;
    }

    static const uint32_t c = 60000000;
    uint32_t bpm = 120; // TODO: retrieve from tempo meta event
    uint32_t uspsqn = c / bpm;
    uint32_t TQPN = midi.header.division.tqpn.TQPN;
    uint32_t usToWait = (e.deltaTime * uspsqn) / TQPN;

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = usToWait * 1000;

    nanosleep(&ts, NULL);

    // TODO: play notes on MIDI device

    eMidi_printMidiEvent(&e);

  } while (!(e.eventId == MIDI_EVENT_META && e.metaEventId == MIDI_END_OF_TRACK));

  eMidi_close(&midi);

  return 0;
}

