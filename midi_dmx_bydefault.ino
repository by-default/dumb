#include <TeensyDMX.h>
#include "midichannel.h"
#include "midimap.h"

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
  usbMIDI.setHandleStop(panic);

  Serial.begin(115200);  // initialize serial:
  Serial.println("Teensy Serial -≥ DMX tester");
  Serial.println("Enter channel number [space] value [newline]");

  dmxTx.setRefreshRate(30); // 30 was ok
  dmxTx.setBreakTime(300); // 300 was ok
  dmxTx.setMABTime(120); //120 was ok
  dmxTx.begin();

  panic();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWriteFast(LED_BUILTIN, HIGH);
}

void loop() {
  byte x = 0;
  byte y = 0;

  while (Serial.available() > 0) {

    x = Serial.parseInt();
    y = Serial.parseInt();

    // sentence:
    if (Serial.read() == '\n') {
      dmxTx.set(x, y);
      //dmxTx.begin();


      Serial.print(x);
      Serial.print(" ");
      Serial.println(y);

      //Serial.print(" ");
      //Serial.println(br1);

    }
  }

  usbMIDI.read();
}
