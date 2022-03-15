#ifndef MIDIFILE_OOP_H
#define MIDIFILE_OOP_H

extern "C" {
#include "midifile.h"
}

//-------------------------------------------------------------------------------------------------
// EmMidiEvent
//-------------------------------------------------------------------------------------------------

class EmMidiEvent {
public:
  EmMidiEvent(MidiFile* pMidiFile, uint8_t eventId, uint32_t absoluteTick)
      : pMidiFile_(pMidiFile), eventId_(eventId), absoluteTick_(absoluteTick) {}

  virtual ~EmMidiEvent() = 0                  { };
  virtual Error write(uint32_t deltaTime) const = 0;

  uint8_t eventId() const                     { return eventId_; }
  uint32_t absoluteTick() const               { return absoluteTick_; }

protected:
  MidiFile* pMidiFile_{nullptr};

private:
  const uint8_t eventId_;
  const uint32_t absoluteTick_;
};

//-------------------------------------------------------------------------------------------------
// EmNotImplementedEvent
//-------------------------------------------------------------------------------------------------

class EmNotImplementedEvent : public EmMidiEvent {
public:
  EmNotImplementedEvent(MidiFile* pMidiFile, uint8_t eventId, uint32_t absoluteTick)
      : EmMidiEvent(pMidiFile, eventId, absoluteTick) {}
  
  Error write(uint32_t deltaTime) const final {
    printf("EmNotImplementedEvent::write()\n"); // TODO: remove
    return EMIDI_OK;
  }
};
#endif // MIDIFILE_OOP_H
