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

//-------------------------------------------------------------------------------------------------
// EmNoteEvent
//-------------------------------------------------------------------------------------------------

class EmNoteEvent : public EmMidiEvent {
public:
  EmNoteEvent(MidiFile* pMidiFile, uint8_t eventId, uint32_t absoluteTick, uint8_t channel, uint8_t note,
      uint8_t velocity)
    : EmMidiEvent(pMidiFile, eventId, absoluteTick), channel_(channel), note_(note), velocity_(velocity) {}
  
  Error write(uint32_t deltaTime) const override = 0;

  uint8_t channel()  const                    { return channel_; }
  uint8_t note()     const                    { return note_; }
  uint8_t velocity() const                    { return velocity_; }

private:
  const uint8_t channel_;
  const uint8_t note_;
  const uint8_t velocity_;
};

//-------------------------------------------------------------------------------------------------
// EmNoteOffEvent
//-------------------------------------------------------------------------------------------------

class EmNoteOffEvent : public EmNoteEvent {
public:
  EmNoteOffEvent(MidiFile* pMidiFile, uint32_t absoluteTick, uint8_t channel, uint8_t note, uint8_t velocity)
      : EmNoteEvent(pMidiFile, MIDI_EVENT_NOTE_OFF, absoluteTick, channel, note, velocity) {}
  
  Error write(uint32_t deltaTime) const final;
};

//-------------------------------------------------------------------------------------------------
// EmNoteOnEvent
//-------------------------------------------------------------------------------------------------

class EmNoteOnEvent : public EmNoteEvent {
public:
  EmNoteOnEvent(MidiFile* pMidiFile, uint32_t absoluteTick, uint8_t channel, uint8_t note, uint8_t velocity)
      : EmNoteEvent(pMidiFile, MIDI_EVENT_NOTE_ON, absoluteTick, channel, note, velocity) {}
  
  Error write(uint32_t deltaTime) const final;
};

//-------------------------------------------------------------------------------------------------
// EmMetaEvent
//-------------------------------------------------------------------------------------------------

class EmMetaEvent : public EmMidiEvent {
public:
  EmMetaEvent(MidiFile* pMidiFile, uint8_t metaEventId, uint32_t absoluteTick)
      : EmMidiEvent(pMidiFile, MIDI_EVENT_META, absoluteTick), metaEventId_(metaEventId) {}
    
  uint8_t metaEventId() const { return metaEventId_; }
  Error write(uint32_t deltaTime) const override = 0;

private:
  const uint8_t metaEventId_;
};

//-------------------------------------------------------------------------------------------------
// EmMetaNotImplementedEvent
//-------------------------------------------------------------------------------------------------

class EmMetaNotImplementedEvent : public EmMetaEvent {
public:
  EmMetaNotImplementedEvent(MidiFile* pMidiFile, uint8_t metaEventId, uint32_t absoluteTick)
    : EmMetaEvent(pMidiFile, metaEventId, absoluteTick) {}
  
  Error write(uint32_t deltaTime) const final {
    printf("EmMetaNotImplementedEvent::write()\n"); // TODO: remove
    return EMIDI_OK;
  }
};

//-------------------------------------------------------------------------------------------------
// EmMetaSetTempoEvent
//-------------------------------------------------------------------------------------------------

class EmMetaSetTempoEvent : public EmMetaEvent {
public:
  EmMetaSetTempoEvent(MidiFile* pMidiFile, uint32_t absoluteTick, float bpm)
      : EmMetaEvent(pMidiFile, MIDI_SET_TEMPO, absoluteTick), bpm_(bpm) {}
  
  Error write(uint32_t deltaTime) const final;
  float bpm() const                         { return bpm_; }

private:
  const float bpm_;
};

#endif // MIDIFILE_OOP_H
