#define _DEFAULT_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

int eMidi_timeUs() {
  return eMidi_halTimeUs();
}

FILE* eMidi_fopen(const char* pPathName, const char* pMode) {
  return fopen(pPathName, pMode);
}

int eMidi_fclose(FILE* pStream) {
  return fclose(pStream);  
}

long eMidi_ftell(FILE* pStream) {
  return ftell(pStream);
}

int eMidi_fseek(FILE* pStream, long offset, int whence) {
  return fseek(pStream, offset, whence);
}

size_t eMidi_fread(void* p, size_t size, size_t nmemb, FILE* pStream) {
  return fread(p, size, nmemb, pStream);
}

size_t eMidi_fwrite(const void* p, size_t size, size_t nmemb, FILE* pStream) {
  return fwrite(p, size, nmemb, pStream);
}

int eMidi_printf(const char* pFormat, ...) {
  va_list args;

  va_start(args, pFormat);
  int ret = vprintf(pFormat, args);
  va_end(args);

  return ret;
}
