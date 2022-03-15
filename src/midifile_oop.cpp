#include "midifile_oop.h"

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
// EmMetaSetTempoEvent
//-------------------------------------------------------------------------------------------------

Error EmMetaSetTempoEvent::write(uint32_t deltaTime) const {
  return eMidi_writeSetTempoMetaEvent(pMidiFile_, deltaTime, bpm());
}
