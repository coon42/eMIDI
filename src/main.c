#include "midifile.h"
#include "byteswap.h"

const char* getErrorDescription(Error error) {
  switch(error) {
    case EMIDI_OK:                      return "Ok";
    case EMIDI_INVALID_HANDLE:          return "Invalid Handle";
    case EMIDI_CANNOT_OPEN_FILE:        return "Cannot open file";
    case EMIDI_INVALID_MIDI_FILE:       return "Invalid MIDI file";
    case EMIDI_UNEXPECTED_END_OF_FILE:  return "Unexpected end of File";
    default:                            return "Unknown";
  }
}

void printError(Error error) {
  printf("Error %d: %s\n", error, getErrorDescription(error));
}

void printMidiFileInfo(MidiFile* pMidiFile) {
  printf("File size: %d bytes\n", pMidiFile->size);
  printf("\n");
  printf("MIDI header:\n");
  printf("------------\n");
  printf("Header Length: %d\n", pMidiFile->header.length);
  printf("Format: %d\n", pMidiFile->header.format);
  printf("Number of Tracks: %d\n", pMidiFile->header.ntrks);
  printf("Division: %d", pMidiFile->header.division);  
}

int main(int argc, char* pArgv[]) {
  if(argc < 2) {
    printf("Usage: eMidi.o <midi file>");

    return 1;
  }

  const char* pMidiFileName = pArgv[1];
  MidiFile midi;  
  Error error;

  error = eMidi_open(&midi, pMidiFileName);

  if(error) {
    printf("Cannot open file: '%s'\n", pMidiFileName);
    printError(error);

    return 2;
  }

  printf("Midi file '%s' opened successfully!\n", pMidiFileName);
  printMidiFileInfo(&midi);

  eMidi_close(&midi);

  return 0;
}

