#include "helpers.h"

int main(int argc, char* pArgv[]) {
  Error error;
  MidiFile midi;

  if (error = eMidi_create(&midi, "example.mid")) {
    eMidi_printError(error);
    return 1;
  }

  if (error = eMidi_writeSetTempoMetaEvent(&midi, 0, 400))
    return error;

  if (error = eMidi_writeNoteOnEvent(&midi, 0, 0, 48, 127))
    return error;

  if (error = eMidi_writeNoteOnEvent(&midi, 960, 0, 64, 127))
    return error;

  if (error = eMidi_writeNoteOnEvent(&midi, 960, 0, 96, 127))
    return error;

  if (error = eMidi_writeNoteOnEvent(&midi, 960, 0, 48, 0))
    return error;

  if (error = eMidi_close(&midi)) {
    eMidi_printError(error);
    return 2;
  }

  return 0;
}

