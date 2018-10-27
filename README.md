# eMIDI [![Development stadium](https://img.shields.io/badge/stadium-pre--alpha-red.svg)](https://coveralls.io/repos/github/coon42/eMIDI/badge.svg?branch=master) [![Build Status](https://travis-ci.org/coon42/eMIDI.svg?branch=master)](https://travis-ci.org/coon42/eMIDI) [![Coverage Status](https://coveralls.io/repos/github/coon42/eMIDI/badge.svg?branch=master)](https://coveralls.io/github/coon42/eMIDI?branch=master)

MIDI library optimized for beeing used on embedded devices

**This library is in a usable state but not recommended for productive use yet! Most events are not supported yet and the API meight break at any time**

### Overall Design Goals ###
  * The main target are microcontrollers with very limited ressources

  * Robust design / does not behave undefined on malformed or broken MIDI files
  * Implementation is done after MIDI 1.0 specification
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
  * Reading of all midi events of any midi0 file (data bytes of some events are ignored)
  * Support of running status events
  * Non-Blocking MIDI player
  * Supported channel events
    * NoteOff
    * NoteOn
    * Control Change
    * Program Change
    * Pitch Bend
  * Supported META events
    * End of Track
    * Set Tempo
    * Midi Port
    * All other events are ignored in first iteration and will be supported later

### Planned ###
  * A cache for the MIDI player to avoid playback timing issues due to slow media access
  * Creation of MIDI files
  * Support of MIDI 1 files
  * MIDI 1 to MIDI 0 converter
  * "Compression" - Use running status events plus Note-On with zero velocity for Note-Off

### Contraints ###
  * Only little endian platforms supported
  
### Using on Arch Linux ###
Development and testing is done using Arch Linux. Even this library is targeted for embedded devices it can also be used on non-embedded devices like windows and linux.

#### Build ####
Just execute **Make**

You will find some examples in **utils** directory:
  * dump: dumps all events of a MIDI file
  * player: plays a MIDI file using fluidsynth
  * gcode: converts a MIDI0 file into gcode, so it can be used for playing music on a CNC mill or 3D printer. (Under development)

To play a MIDI file just execute

```bash
bin/player tests/midis/cdefgabc_0.mid
```


If you get an error like: 

**Error: cannot open /dev/sequencer**

You might need to install fluidsynth first:

#### Install fluidsynth ####

```bash
sudo pacman -S fluidsynth
```

#### Install a soundfont ####
```bash
sudo pacman -S soundfont-fluid
```

#### Setup daemon for MIDI playback ####
```bash
sudo vim /etc/conf.d/fluidsynth 
```

Replace the contents by:

```
SOUND_FONT=/usr/share/soundfonts/FluidR3_GM.sf2
AUDIO_DRIVER=alsa
OTHER_OPTS='-is -m alsa_seq -r 48000'
```

#### Enable and start the fluidsynth daemon ####
Create a file at 
```
~/.config/systemd/user/fluidsynth.service
```

**You meight create the systemd/user directories first!**

Insert the following content:

```ini
[Unit]
Description=FluidSynth launched in server mode
After=sound.target

[Service]
ExecStart=/usr/bin/fluidsynth -a pulseaudio -m alsa_seq -i -l -s -p FluidSynth /usr/share/soundfonts/FluidR3_GM.sf2

[Install]
WantedBy=default.target
```

Execute:

```bash
systemctl --user start fluidsynth.service
systemctl --user enable fluidsynth.service
```

If /dev/sequencer is not showing up you also meight execute:

```bash
sudo modprobe snd-seq-oss
```

And let is start on each boot by opening 

```
/etc/modules-load.d/modules.conf
```

and adding:
```
snd-seq-oss
```
