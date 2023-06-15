#include <stdbool.h>

/*
  each entry of MIDI map
  {MIDI channel, MIDI note, DMX channel, parameter}

  parameter is:
    DEFAULT_VALUE - velocity pass to DMX channel
    LIMIT(0...255) - limit maximum value of DMX
    DISCRETE - DMX set to 255 value by NOTEON, set to 0 by NOTEOFF, ignore velocity
    CONSTANT(x) - DMX set to x value by NOTEON, set to 0 by NOTEOFF, ignore velocity

  example for dimmer:
  {1, 20, 100, DEFAULT_VALUE} -- pass velocity of note 20, channel 1 to dimmer on DMX channel 100

  example for dimmer with limited output:
  {1, 21, 103, LIMIT(120)} -- pass velocity of note 21, channel 1 to dimmer on DMX channel 105,
  dimmer value is no more 120

  example for relay:
  {1, 22, 50, DISCRETE} -- pass note 22, channel 1 to on/off relay on DMX channel 50

  example for video player:
  notes 61..64 NOTEON on channel 2 starts video 1..4 on DMX channel 10.
  note 60 stop video

  {2, 60, 10, CONSTANT(0)}
  {2, 61, 10, CONSTANT(1)}
  {2, 62, 10, CONSTANT(2)}
  {2, 63, 10, CONSTANT(3)}
  {2, 64, 10, CONSTANT(4)}
*/

typedef struct
{
  bool isConstant;
  bool handleOff;
  int value;
} ChannelParameters;

typedef struct {
  int channel;
  int note;
  int address;
  ChannelParameters param;
} Midichannel;

#define DEFAULT_VALUE {false,true,255}
#define LIMIT(x) {false,true,x}
#define DISCRETE {true,true,255}
#define CONSTANT(x) {true,false,x}

#define MIDI_CC(x) (x + 127)
