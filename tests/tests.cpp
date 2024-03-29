#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "Catch2/single_include/catch.hpp"

extern "C" {
  #include "../src/midifile.h"
}

// TODO: try to use API function on a file with wrong mode
//       e.g. eMidi_create() followed by eMidi_readEvent()

TEST_CASE("eMidi_open tests", "[open]") {
  MidiFile midi;

  SECTION("providing NULL for pMidiFile and NULL for pFileName") {
    REQUIRE(eMidi_open(NULL, NULL) == EMIDI_INVALID_HANDLE);
  }

  SECTION("providing valid object for pMidiFile and NULL for pFileName") {
    REQUIRE(eMidi_open(&midi, NULL) == EMIDI_CANNOT_OPEN_FILE);
  }

  SECTION("providing valid object for pMidiFile and file name of invalid MIDI file") {
    REQUIRE(eMidi_open(&midi, "midis/invalid.mid") == EMIDI_INVALID_MIDI_FILE);
  }

  SECTION("providing valid object for pMidiFile and file name of valid MIDI 0 file") {
    REQUIRE(eMidi_open(&midi, "midis/c4_0.mid") == EMIDI_OK);
  }

  SECTION("open valid MIDI 0 file and check for correct file mode") {
    REQUIRE(eMidi_open(&midi, "midis/c4_0.mid") == EMIDI_OK);
    REQUIRE(midi.mode == MIDI_FILE_MODE_READ);
  }
}

TEST_CASE("eMidi_readEvent tests", "[open, readevent]") {
  MidiFile midi;
  Error eror;
  MidiEvent e;

  SECTION("Complete parsing of cdefgabc_0.mid without any error") {
    REQUIRE(eMidi_open(&midi, "midis/cdefgabc_0.mid") == EMIDI_OK);

    do {
      REQUIRE(eMidi_readEvent(&midi, &e) == EMIDI_OK);
    } while (!(e.eventId == MIDI_EVENT_META && e.metaEventId == MIDI_END_OF_TRACK));
  }

  SECTION("Complete parsing of turkish_match0.mid without any error") {
    REQUIRE(eMidi_open(&midi, "midis/turkish_march0.mid") == EMIDI_OK);

    do {
      REQUIRE(eMidi_readEvent(&midi, &e) == EMIDI_OK);
    } while (!(e.eventId == MIDI_EVENT_META && e.metaEventId == MIDI_END_OF_TRACK));
  }

  SECTION("Complete parsing of vivaldi_spring0.mid without any error") {
    REQUIRE(eMidi_open(&midi, "midis/vivaldi_spring0.mid") == EMIDI_OK);

    do {
      REQUIRE(eMidi_readEvent(&midi, &e) == EMIDI_OK);
    } while (!(e.eventId == MIDI_EVENT_META && e.metaEventId == MIDI_END_OF_TRACK));
  }

  SECTION("Complete parsing of child_in_time0.mid without any error") {
    REQUIRE(eMidi_open(&midi, "midis/child_in_time0.mid") == EMIDI_OK);

    do {
      REQUIRE(eMidi_readEvent(&midi, &e) == EMIDI_OK);
    } while (!(e.eventId == MIDI_EVENT_META && e.metaEventId == MIDI_END_OF_TRACK));
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
// - End of Track event must appear as last event
// - The optional events "Sequence Number" and "Sequence Track Name" must appear in tack 0, if used
// - Start a sysex message F0 but never terminate it with F7
// - running status event tests:
// -   only allow running status for voice and mode messages only
// - Files with SMPTE divison must result in a EMIDI_DIVISION_FORMAT_NOT_SUPPORTED error on opening

}

TEST_CASE("eMidi_create tests", "[create]") {
  MidiFile midi;

  SECTION("Create a MIDI file and directly close it again") {
    REQUIRE(eMidi_create(&midi, "test.mid", MIDI_DEFAULT_TPQN) == EMIDI_OK);
    REQUIRE(midi.mode == MIDI_FILE_MODE_CREATE);
    REQUIRE(eMidi_close(&midi) == EMIDI_OK);
  }
}

