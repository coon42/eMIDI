cmake_minimum_required(VERSION 3.10.3)

include_directories(src)

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

