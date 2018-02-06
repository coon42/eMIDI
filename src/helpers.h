#ifndef _HELPERS_H
#define _HELPERS_H

const char* eMidi_errorToStr(Error error);
const char* eMidi_eventToStr(uint8_t eventCode);
const char* eMidi_metaEventToStr(int metaEventId);

Error eMidi_printMidiEvent(const MidiEvent* e);
Error eMidi_printError(Error error);
Error eMidi_printFileInfo(MidiFile* pMidiFile);

#endif // _HELPERS_H

