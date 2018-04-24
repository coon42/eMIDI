#define _DEFAULT_SOURCE
#include <unistd.h>

#include <stdlib.h>
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

static uint32_t prvGetFileSize(FILE* p) {
  eMidi_fseek(p, 0, SEEK_END);
  uint32_t fileSize = eMidi_ftell(p);
  eMidi_fseek(p, 0, SEEK_SET);

  return fileSize;
}

static void writeArray(FILE* pMidiFile, FILE* pSourceFile) {
  uint32_t fileSize = prvGetFileSize(pMidiFile);
  char* p = malloc(fileSize + sizeof(uint32_t));
  memcpy(p, &fileSize, sizeof(uint32_t));

  eMidi_fread(&p[4], 1, fileSize, pMidiFile);

  writeStr(pSourceFile, "const unsigned char pMidi[] = {");

  for(int i = 0; i < fileSize + sizeof(uint32_t); ++i) {
    if(i % 20 == 0) // limit text to about 100 columns
      writeStr(pSourceFile, "\n  ");

    char pNum[32];
    snprintf(pNum, sizeof(pNum), "0x%02X,", (unsigned char)p[i]);

    writeStr(pSourceFile, pNum);
  }

  writeStr(pSourceFile, "\n");
  writeStr(pSourceFile, "};\n");

  if(p)
    free(p);
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

