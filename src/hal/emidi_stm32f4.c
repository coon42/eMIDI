#define _DEFAULT_SOURCE
#include <unistd.h>
#include <time.h>
#include <stdio.h>

int eMidi_timeUs() {
  // TODO: implement

  return 0;
}

FILE* eMidi_fopen(const char* pPathName, const char* pMode) {
  return NULL; // TODO: implement
}

int eMidi_fclose(FILE* pStream) {
  return 0; // TODO: implement
}

long eMidi_ftell(FILE* pStream) {
  return 0; // TODO: implement
}

int eMidi_fseek(FILE* pStream, long offset, int whence) {
  return 0; // TODO: implement
}

size_t eMidi_fwrite(const void* p, size_t size, size_t nmemb, FILE* pStream) {
  return fwrite(p, size, nmemb, pStream);
}

int eMidi_printf(const char* pFormat, ...) {
  return 0; // TODO: implement
}


