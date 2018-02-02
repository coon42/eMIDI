# eMIDI
MIDI library optimized for beeing used on embedded devices

**This library is in a very early development state and not recommended for use yet!**

### Overall Design Goals ###
  * The main target are microcontrollers with very limited ressources

  * Robust design / does not behave undefined on malformed or broken MIDI files
  * Implementation is strictly done after MIDI 1.0 specification
  * Minimum code footprint (configurable by #define)
  * Minimum RAM footprint (configurable by #define)
  * MIDI file gets not loaded into memory but gets streamed
  * A lock free buffer cache which is filled while playing to avoid real time issues on playback
  * No use of dynamic memory (malloc and free), just static memory 
  * Simple abstraction layer, so the library can be ported to any platform easily

  * In a later development state MIDI files can also be created. This allows implementing a
    simple sequencer for example.

### Implemented features so far ###

  * Opening and closing of MIDI files and validation of header.
  * Reading of all midi events of any midi0 file (data bytes are ignored / running status does not work yet)
  * Support of running status events

### Planned ###
  * In first iteration only MIDI 0 files are supported. Other formats will also be supported later.

  * Basic support MIDI events
    * NoteOn
    * NoteOff
    * End of Track meta event
    * All other events are ignored in first iteration and will be supported later
 
### Contraints ###
  * Only little endian platforms supported

