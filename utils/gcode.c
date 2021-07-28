#define _DEFAULT_SOURCE
#include <unistd.h>

#include <string.h>
#include "midifile.h"
#include "helpers.h"
#include "hal/emidi_hal.h"

static void writeCmd(FILE* p, const char* pCmd) {
  eMidi_fwrite(pCmd, 1, strlen(pCmd), p);
  eMidi_fwrite("\r\n", 1, 2, p);
}

static void writeMidiNote(FILE* p, uint8_t axisId, uint8_t note, uint32_t durationUs) {
  // TODO: implement
}

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

  FILE* p = eMidi_fopen("gcode.nc", "w");

  writeCmd(p, "G21"); // Unit selection: millimeters
  writeCmd(p, "G90"); // Absolute distance mode
  writeCmd(p, "G94"); // Feedrate is units per minute
  writeCmd(p, "G00 X0 Y0 Z0"); // Go to home position

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
        uspqn = e.params.msg.meta.setTempo.usPerQuarterNote;
    }

    uint32_t TPQN = midi.header.division.tpqn.TPQN;
    uint32_t usToWait = (e.deltaTime * uspqn) / TPQN;

    // TODO: generate G1 gcode commands:
    usleep(usToWait);
    eMidi_printMidiEvent(&e);

  } while (!(e.eventId == MIDI_EVENT_META && e.metaEventId == MIDI_END_OF_TRACK));

  writeCmd(p, "M02");
  writeCmd(p, "");

  eMidi_fclose(p);
  eMidi_close(&midi);

  return 0;
}

