#ifndef EMIDI_HAL_H
#define EMIDI_HAL_H

// Timing:
int eMidi_halTimeUs();

// File access:
FILE* eMidi_halFopen(const char* pPathName, const char* pMode);
int eMidi_halFclose(FILE* pStream);
long eMidi_halFtell(FILE* pStream);
int eMidi_halFseek(FILE* pStream, long offset, int whence);

#endif // EMIDI_HAL_H

