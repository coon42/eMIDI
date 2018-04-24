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

static void writeStr(FILE* pSourceFile, const char* pStr) {
  eMidi_fwrite(pStr, 1, strlen(pStr), pSourceFile);
}

static void writeArray(FILE* pMidiFile, FILE* pSourceFile) {
  writeStr(pSourceFile, "const unsigned char pMidi[] = {");

  unsigned char b;

  for(int i = 0; eMidi_fread(&b, 1, 1, pMidiFile) == 1; ++i) {
    if(i % 20 == 0) // limit text to about 100 columns
      writeStr(pSourceFile, "\n  ");

    char pNum[32];
    snprintf(pNum, sizeof(pNum), "0x%02X,", (int)b);

    writeStr(pSourceFile, pNum);
  }

  writeStr(pSourceFile, "\n");
  writeStr(pSourceFile, "};\n");
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

