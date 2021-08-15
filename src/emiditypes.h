#ifndef EMIDI_TYPES_H
#define EMIDI_TYPES_H

#include <stdint.h>

enum {
  EMIDI_OK                     = 0x0000,
  EMIDI_OK_END_OF_FILE         = 0x0001,
  EMIDI_OK_END_OF_PORTS        = 0x0002,

  EMIDI_INVALID_HANDLE         = 0x1000,
  EMIDI_CANNOT_OPEN_FILE       = 0x1001,
  EMIDI_INVALID_FILE_MODE      = 0x1002,
  EMIDI_INVALID_MIDI_FILE      = 0x1003,
  EMIDI_SYNTAX_ERROR           = 0x1004,
  EMIDI_UNEXPECTED_END_OF_FILE = 0x1005,

  EMIDI_CANNOT_WRITE_TO_FILE   = 0x1006,
  EMIDI_FILE_NOT_OPENED        = 0x1007,

  EMIDI_INVALID_PORT_INDEX     = 0x1100,
  EMIDI_INVALID_CHANNEL        = 0x1200,
  EMIDI_INVALID_NOTE           = 0x1201,
  EMIDI_INVALID_VELOCITY       = 0x1202,
  EMIDI_INVALID_CONTROL        = 0x1203,
  EMIDI_INVALID_BPM            = 0x1204,

  // remove as soon as supported:
  EMIDI_FUNCTION_NOT_IMPLEMENTED      = 0x2000,
  EMIDI_FORMAT_1_NOT_SUPPORTED        = 0x2001,
  EMIDI_FORMAT_2_NOT_SUPPORTED        = 0x2002,
  EMIDI_DIVISION_FORMAT_NOT_SUPPORTED = 0x2003
};

typedef uint32_t Error;

#define checkChannelParam(channel) if (channel > 15) return EMIDI_INVALID_CHANNEL
#define checkNoteParam(note) if (note > 127) return EMIDI_INVALID_NOTE
#define checkVelocityParam(velocity) if (velocity > 127) return EMIDI_INVALID_VELOCITY
#define checkControlParam(control) if (control > 127) return EMIDI_INVALID_CONTROL
#define checkBpmParam(bpm) if (bpm == 0) return EMIDI_INVALID_BPM

#endif // EMIDI_TYPES_H
