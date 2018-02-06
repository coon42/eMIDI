#include "midifile.h"
#include "helpers.h"

static void printMidiFileEvents(MidiFile* pMidiFile) {
  printf("Now reading MIDI events:\n");
  printf("========================\n\n");
  MidiEvent e;
  Error error;

  do {
    if(error = eMidi_readEvent(pMidiFile, &e)) {
      printf("Error on reading event: [0x%02X] (Error %d: %s)\n",e.eventId, error, eMidi_errorToStr(error));
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

  return 0;
}

