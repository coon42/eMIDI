#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch.hpp"

extern "C" {
  #include "../src/midifile.h"
}

TEST_CASE("eMidi_open tests", "[open]") {
  SECTION("providing NULL for pMidiFile and NULL for pFileName") {
    REQUIRE(eMidi_open(NULL, NULL) == EMIDI_INVALID_HANDLE);
  }

  SECTION("providing valid object for pMidiFile and NULL for pFileName") {
    MidiFile midi;

    REQUIRE(eMidi_open(&midi, NULL) == EMIDI_CANNOT_OPEN_FILE);
  }

  SECTION("providing valid object for pMidiFile and file name of invalid MIDI File") {
    MidiFile midi;

    REQUIRE(eMidi_open(&midi, "midis/invalid.mid") == EMIDI_INVALID_MIDI_FILE);
  }

  SECTION("providing valid object for pMidiFile and file name of valid MIDI 0 File") {
    MidiFile midi;

    REQUIRE(eMidi_open(&midi, "midis/c4_0.mid") == EMIDI_OK);
  }

// TODO: convert c4_0.mid to Format 1 and Format 2:

/*
  SECTION("providing valid object for pMidiFile and file name of valid MIDI 1 File") {
    MidiFile midi;

    REQUIRE(eMidi_open(&midi, "c4_1.mid") == EMIDI_FORMAT_1_NOT_SUPPORTED);
  }

  SECTION("providing valid object for pMidiFile and file name of valid MIDI 2 File") {
    MidiFile midi;

    REQUIRE(eMidi_open(&midi, "c4_2.mid") == EMIDI_FORMAT_2_NOT_SUPPORTED);
  }
*/

// TODO: create invalid MIDI files with following behaviour:
// - ntrks is != 1 on when format is 0 
// - format is greater than 2
// - unexpected end of file
// - No MTrk header after MThd header
// - No event after MTrk header

}

