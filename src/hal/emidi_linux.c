#define _DEFAULT_SOURCE
#include <unistd.h>
#include <time.h>

int eMidi_halTimeUs() {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);

  return t.tv_sec * 1000000 + t.tv_nsec / 1000;
}

