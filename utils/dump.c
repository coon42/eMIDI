#include "midifile.h"
#include "helpers.h"

static void usToTimeStr(uint32_t us, char* pBuf, int bufLen) {
  uint32_t m  = (us / 60) / 1000000;
  uint32_t s  = us        / 1000000 - m * 60;
  // uint32_t ms = ((us - m * 60 * 1000000 - s * 1000000) / 100 + 5) / 10;
  uint32_t ms = (us - m * 60 * 1000000 - s * 1000000) / 1000; // TODO: replace by above (round)

  snprintf(pBuf, bufLen, "%02d:%02d:%03d", m, s, ms);
}

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

    usTotal += (e.deltaTime * uspqn) / pMidiFile->header.division.tpqn.TPQN;

    if(e.eventId == MIDI_EVENT_META) {
      if(e.metaEventId == MIDI_SET_TEMPO)
        uspqn = e.params.msg.meta.setTempo.usPerQuarterNote;
    }

  } while (!(e.eventId == MIDI_EVENT_META && e.metaEventId == MIDI_END_OF_TRACK));

  char pBuf[64];
  usToTimeStr(usTotal, pBuf, sizeof(pBuf));

  printf("Duration: %s\n", pBuf);
}

static void printMidiFileEvents(MidiFile* pMidiFile) {
  printf("Now reading MIDI events:\n");
  printf("========================\n\n");
  MidiEvent e;
  Error error;

  uint32_t totalTicks = 0;
  static const uint32_t c = 60000000;
  uint32_t bpm = 120;
  uint32_t uspqn = c / bpm;
  uint32_t usTotal = 0;

  do {
    if(error = eMidi_readEvent(pMidiFile, &e)) {
      printf("Error on reading event: [0x%02X] (Error %d: %s)\n",e.eventId, error,
          eMidi_errorToStr(error));
      return;
    }

    if(e.eventId == MIDI_EVENT_META) {
      if(e.metaEventId == MIDI_SET_TEMPO)
        uspqn = e.params.msg.meta.setTempo.usPerQuarterNote;
    }

    usTotal += (e.deltaTime * uspqn) / pMidiFile->header.division.tpqn.TPQN;

    char pBuf[64];
    usToTimeStr(usTotal, pBuf, sizeof(pBuf));

    printf("[%04d (%s)] ", totalTicks, pBuf);
    eMidi_printMidiEvent(&e);

    totalTicks += e.deltaTime;

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

