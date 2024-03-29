#ifndef _HELPERS_H
#define _HELPERS_H

#include <stdint.h>
#include "emiditypes.h"
#include "midifile.h"

const char* eMidi_errorToStr(Error error);
const char* eMidi_eventToStr(uint8_t eventCode);
const char* eMidi_metaEventToStr(int metaEventId);
const char* eMidi_numberToNote(uint8_t midiNote);
const char* eMidi_drumToStr(uint8_t midiNote);
const char* eMidi_controllerToStr(uint8_t controllerNo);
const char* eMidi_programToStr(uint8_t programNo);

Error eMidi_printMidiEvent(const MidiEvent* e);
void eMidi_printError(Error error);
Error eMidi_printFileInfo(const MidiFile* pMidiFile);

#endif // _HELPERS_H

