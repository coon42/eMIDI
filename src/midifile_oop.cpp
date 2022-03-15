#include "midifile_oop.h"

//-------------------------------------------------------------------------------------------------
// EmMidiEvent
//-------------------------------------------------------------------------------------------------

EmMidiEvent::~EmMidiEvent() {

}

//-------------------------------------------------------------------------------------------------
// EmNoteOffEvent
//-------------------------------------------------------------------------------------------------

Error EmNoteOffEvent::write(uint32_t deltaTime) const {
  return eMidi_writeNoteOffEvent(pMidiFile_, deltaTime, channel(), note(), velocity());
}

//-------------------------------------------------------------------------------------------------
// EmNoteOnEvent
//-------------------------------------------------------------------------------------------------

Error EmNoteOnEvent::write(uint32_t deltaTime) const {
  return eMidi_writeNoteOnEvent(pMidiFile_, deltaTime, channel(), note(), velocity());
}

//-------------------------------------------------------------------------------------------------
// EmProgramChangeEvent
//-------------------------------------------------------------------------------------------------

Error EmProgramChangeEvent::write(uint32_t deltaTime) const {
  return eMidi_writeProgramChangeEvent(pMidiFile_, deltaTime, channel(), programNumber());
}

//-------------------------------------------------------------------------------------------------
// EmMetaSetTempoEvent
//-------------------------------------------------------------------------------------------------

Error EmMetaSetTempoEvent::write(uint32_t deltaTime) const {
  return eMidi_writeSetTempoMetaEvent(pMidiFile_, deltaTime, bpm());
}
