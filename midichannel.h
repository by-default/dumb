#include <stdbool.h>

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
