#define _DEFAULT_SOURCE
#include <unistd.h>

#include <string.h>
#include "midifile.h"
#include "helpers.h"
#include "hal/emidi_hal.h"

static Error checkIfValidMidiFile(const char* pFileName) {
  MidiFile midi;
  Error error;

  if(error = eMidi_open(&midi, pFileName))
    return error;

  eMidi_close(&midi);

  return EMIDI_OK;
}

static void writeArray(const FILE* pMidiFile, FILE* pSourceFile) {
  // TODO: implement
}

int main(int argc, char* pArgv[]) {
  if(argc < 2) {
    printf("Usage: midi2array <midi file>");

    return 1;
  }

  const char* pMidiFileName = pArgv[1];
  Error error;

  if(error = checkIfValidMidiFile(pMidiFileName)) {
    printf("Cannot open file: '%s'\n", pMidiFileName);
    eMidi_printError(error);

    return 2;
  }

  FILE* pMidiFile = eMidi_fopen(pMidiFileName, "rb");
  FILE* pSourceFile = eMidi_fopen("array.c", "wb");

  writeArray(pMidiFile, pSourceFile);

  eMidi_fclose(pSourceFile);
  eMidi_fclose(pMidiFile);

  return 0;
}

