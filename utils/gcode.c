#define _DEFAULT_SOURCE
#include <unistd.h>

#include "midifile.h"
#include "helpers.h"

int main(int argc, char* pArgv[]) {
  if(argc < 2) {
    printf("Usage: gcode <midi file>");

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

  static const uint32_t c = 60000000;
  uint32_t bpm = 120;
  uint32_t uspqn = c / bpm;

  do {
    if(error = eMidi_readEvent(&midi, &e)) {
      printf("Error on reading event: [0x%02X] (Error %d: %s)\n",e.eventId, error, eMidi_errorToStr(error));
      return 3;
    }

    if(e.eventId == MIDI_EVENT_META) {
      if(e.metaEventId == MIDI_SET_TEMPO)
        uspqn = e.params.meta.setTempo.usPerQuarterNote;
    }

    uint32_t TQPN = midi.header.division.tqpn.TQPN;
    uint32_t usToWait = (e.deltaTime * uspqn) / TQPN;

    // TODO: generate G1 gcode command:
    usleep(usToWait);
    eMidi_printMidiEvent(&e);

  } while (!(e.eventId == MIDI_EVENT_META && e.metaEventId == MIDI_END_OF_TRACK));

  eMidi_close(&midi);

  return 0;
}

