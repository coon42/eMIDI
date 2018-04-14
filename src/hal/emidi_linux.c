#define _DEFAULT_SOURCE
#include <unistd.h>
#include <time.h>
#include <stdio.h>

int eMidi_halTimeUs() {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);

  return t.tv_sec * 1000000 + t.tv_nsec / 1000;
}

FILE* eMidi_halFopen(const char* pPathName, const char* pMode) {
  return fopen(pPathName, pMode);
}

int eMidi_halFclose(FILE* pStream) {
  fclose(pStream);
}

long eMidi_halFtell(FILE* pStream) {
  return ftell(pStream);
}

int eMidi_halFseek(FILE* pStream, long offset, int whence) {
  return fseek(pStream, offset, whence);
}

