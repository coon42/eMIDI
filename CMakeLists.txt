cmake_minimum_required(VERSION 3.10.3)
include_directories(src)

option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

#TODO: -std=c99, -std=c++17

project (dump)
add_executable(dump src/midifile.c src/helpers.c src/hal/emidi_linux.c utils/dump.c)

# add HAL switch for different platforms:
project (player)
add_executable(player src/midifile.c src/midiplayer.c src/helpers.c src/hal/emidi_linux.c utils/player.c)

# add HAL switch for different platforms:
project (memplayer)
add_executable(memplayer src/midifile.c src/midiplayer.c src/helpers.c src/hal/emidi_memory.c utils/player.c)

# add HAL switch for different platforms:
project (gcode)
add_executable(gcode src/midifile.c src/helpers.c src/hal/emidi_linux.c utils/gcode.c)

project (midi2array)
add_executable(midi2array src/midifile.c src/helpers.c src/hal/emidi_linux.c utils/midi2array.c)

# TODO: add test target (and execute test after compilation)

