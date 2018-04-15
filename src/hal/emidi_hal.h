#ifndef EMIDI_HAL_H
#define EMIDI_HAL_H

// Timing:
int eMidi_timeUs();

// File access:
FILE* eMidi_fopen(const char* pPathName, const char* pMode);
int eMidi_fclose(FILE* pStream);
long eMidi_ftell(FILE* pStream);
int eMidi_fseek(FILE* pStream, long offset, int whence);

// text output:
int eMidi_printf(const char* pFormat, ...);

#endif // EMIDI_HAL_H

