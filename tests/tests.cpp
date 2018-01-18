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

    REQUIRE(eMidi_open(&midi, "invalid.mid") == EMIDI_INVALID_MIDI_FILE);
  }

  SECTION("providing valid object for pMidiFile and file name of valid MIDI File") {
    MidiFile midi;

    REQUIRE(eMidi_open(&midi, "turkish_march0.mid") == EMIDI_OK);
  }
}

