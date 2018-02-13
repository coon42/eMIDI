#include "midifile.h"
#include "helpers.h"

static void printTotalRuntime(MidiFile* pMidiFile) {
  static const uint32_t c = 60000000;
  uint32_t bpm = 120;
  uint32_t uspqn = c / bpm;
  uint32_t usTotal = 0;

  MidiEvent e;
  Error error;

  do {
    if(error = eMidi_readEvent(pMidiFile, &e)) {
      printf("Error on reading event: [0x%02X] (Error %d: %s)\n",e.eventId, error,
          eMidi_errorToStr(error));
      return;
    }

    if(e.eventId == MIDI_EVENT_META) {
      if(e.metaEventId == MIDI_SET_TEMPO)
        uspqn = e.params.meta.setTempo.usPerQuarterNote;
    }

    usTotal += (e.deltaTime * uspqn) / pMidiFile->header.division.tqpn.TQPN;

  } while (!(e.eventId == MIDI_EVENT_META && e.metaEventId == MIDI_END_OF_TRACK));

  uint32_t m  = (usTotal / 60) / 1000000;
  uint32_t s  = usTotal        / 1000000 - m * 60;
  uint32_t ms = ((usTotal - m * 60 * 1000000 - s * 1000000) / 100 + 5) / 10;

  printf("Duration: %02d:%02d:%03d\n", m, s, ms);
}

static void printMidiFileEvents(MidiFile* pMidiFile) {
  printf("Now reading MIDI events:\n");
  printf("========================\n\n");
  MidiEvent e;
  Error error;

  do {
    if(error = eMidi_readEvent(pMidiFile, &e)) {
      printf("Error on reading event: [0x%02X] (Error %d: %s)\n",e.eventId, error,
          eMidi_errorToStr(error));
      return;
    }

    eMidi_printMidiEvent(&e);

  } while (!(e.eventId == MIDI_EVENT_META && e.metaEventId == MIDI_END_OF_TRACK));
}

int main(int argc, char* pArgv[]) {
  if(argc < 2) {
    printf("Usage: dump <midi file>");

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

  eMidi_printFileInfo(&midi);
  printMidiFileEvents(&midi);
  eMidi_close(&midi);

  printf("======\n");

  eMidi_open(&midi, pMidiFileName);
  printTotalRuntime(&midi);
  eMidi_close(&midi);

  return 0;
}

