#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch.hpp"

extern "C" {
  #include "../src/midifile.h"
}

TEST_CASE( "eMidi_open tests", "[open]" ) {
  REQUIRE( eMidi_open(NULL, NULL) == EMIDI_INVALID_HANDLE );
}

