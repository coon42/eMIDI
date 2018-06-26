#include "helpers.h"

int main(int argc, char* pArgv[]) {
  Error error;
  MidiFile midi;

  if (error = eMidi_create(&midi)) {
    eMidi_printError(error);
    return 1;
  }

  if (error = eMidi_save(&midi, "example.mid")) {
    eMidi_printError(error);
    return 2;
  }

  if(error = eMidi_close(&midi)) {
    eMidi_printError(error);
    return 3;
  }

  return 0;
}

