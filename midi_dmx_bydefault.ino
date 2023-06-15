#include <TeensyDMX.h>
#include "midichannel.h"

/*
  each entry:
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

const Midichannel MAP[] = {
  {1, 20, 100, DEFAULT_VALUE}, // dimmer
  {1, 21, 103, LIMIT(120)}, // dimmer with limited output
  {1, 22, 50, DISCRETE}, // relay

  // video player 1
  {2, 60, 10, CONSTANT(1)},
  {2, 61, 10, CONSTANT(2)},
  {2, 62, 10, CONSTANT(3)},
  {2, 63, 10, CONSTANT(4)},
  {2, 64, 10, CONSTANT(5)},
};

namespace teensydmx = ::qindesign::teensydmx;
teensydmx::Sender dmxTx{Serial1};

void setChannel(int channel, int note, int velocity) {
  // scan for all channel, find matching MIDI channel and note
  for (size_t i = 0; i < sizeof(MAP) / sizeof(MAP[0]); i++) {
    if (channel == MAP[i].channel && note == MAP[i].note) {
      int value = -1;

      if (MAP[i].param.isConstant) {
        if (velocity > 0) {
          value = MAP[i].param.value;
        } else if (MAP[i].param.handleOff) {
          value = 0;
        } else {
          // just ignore noteoff
        }
      } else {
        value = map(velocity, 0, 127, 0, MAP[i].param.value);
        value = constrain(velocity, 0, MAP[i].param.value);
      }

      if (value >= 0) {
        dmxTx.set(MAP[i].address, value);
        //dmxTx.begin();
      }
    }
  }
}

void myNoteOn(byte channel, byte note, byte velocity) {
  digitalWriteFast(LED_BUILTIN, HIGH);
  Serial.print("Note On, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);

  setChannel(channel, note, velocity);
}

void myNoteOff(byte channel, byte note, byte velocity) {
  digitalWriteFast(LED_BUILTIN, LOW);
  Serial.print("Note Off, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);

  setChannel(channel, note, 0);
}

void myControlChange(byte channel, byte control, byte value) {
  digitalWriteFast(LED_BUILTIN, HIGH);
  Serial.print("Control Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", control=");
  Serial.print(control, DEC);
  Serial.print(", value=");
  Serial.println(value, DEC);
  digitalWriteFast(LED_BUILTIN, LOW);

  setChannel(channel, MIDI_CC(control), value);
}

void panic() {
  for (size_t i = 0; i < sizeof(MAP) / sizeof(MAP[0]); i++) {
    dmxTx.set(MAP[i].address, 0);

  }
}

void setup() {
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandleNoteOff(myNoteOff);
  usbMIDI.setHandleControlChange(myControlChange);
  Serial.begin(115200);  // initialize serial:

  dmxTx.setRefreshRate(30); // 30 was ok
  dmxTx.setBreakTime(300); // 300 was ok
  dmxTx.setMABTime(120); //120 was ok
  dmxTx.begin();
  
  panic();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWriteFast(LED_BUILTIN, HIGH);
}

void loop() {
  usbMIDI.read();
}
