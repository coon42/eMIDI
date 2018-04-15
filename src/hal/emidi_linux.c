#define _DEFAULT_SOURCE
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>

int eMidi_timeUs() {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);

  return t.tv_sec * 1000000 + t.tv_nsec / 1000;
}

FILE* eMidi_fopen(const char* pPathName, const char* pMode) {
  return fopen(pPathName, pMode);
}

int eMidi_fclose(FILE* pStream) {
  fclose(pStream);
}

long eMidi_ftell(FILE* pStream) {
  return ftell(pStream);
}

int eMidi_fseek(FILE* pStream, long offset, int whence) {
  return fseek(pStream, offset, whence);
}

size_t eMidi_fwrite(const void* p, size_t size, size_t nmemb, FILE* pStream) {
  return fwrite(p, size, nmemb, pStream);
}

int eMidi_printf(const char* pFormat, ...) {
  va_list args;
  va_start(args, pFormat);

  vprintf(pFormat, args);

  va_end(args);
}

