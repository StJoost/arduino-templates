//Template for sending analog & digital sensors through MIDI to resolume / ableton / ....
//By Thomas Rutgers - Techlab St. Joost - 2022

//Digital pins send note on/off when connecting/disconnecting to ground
//Analog pins send controller data, and can be configured with min. and max, or boolean threshhold value
//Midi is sent on signal change: connect unused analog inputs to ground

#include "MIDIUSB.h"

//analog Sensor Configuration
const int sensorData[] = {
  A0, 0, 1023, 0, //pin number, min, max, thresh
  A1, 0, 1023, 0,
  A2, 0, 1023, 0,
};

int nrOfSensors = sizeof(sensorData) / (sizeof(int)*4);
int reading[sizeof(sensorData) / (sizeof(int)*4)];
int lastReading[sizeof(sensorData) / (sizeof(int)*4)];
int digitalReading[14];
int lastDigitalReading[14];
void setup() {
  Serial.begin(115200);
  for (int n=2; n<=14; n++){
    pinMode(n,INPUT_PULLUP);
    lastDigitalReading[n] = digitalRead(n);
  }
  for (int n=0; n< nrOfSensors; n++) {
    lastReading[n] = analogRead(A0+n);
  }
}


void loop() {
  for (int n=0; n< nrOfSensors; n++) {
    if (sensorData[(n*4)+3] == 0) reading[n] = map(analogRead(sensorData[n*4]),sensorData[(n*4)+1],sensorData[(n*4)+2], 0, 127);
    else reading[n] = (analogRead(sensorData[n*4])>sensorData[(n*4)+3])?127:0;
    reading[n] = constrain(reading[n],0,127);
    if (lastReading[n] != reading[n]) {
        controlChange(0, n, reading[n]);
    }
    lastReading[n] = reading[n];
  }

  for (int n=2; n< 14; n++) {
    digitalReading[n] = digitalRead(+n);
    if (lastDigitalReading[n] != digitalReading[n]) {
        noteOn(0, n, !digitalReading[n] * 127);
    }
    lastDigitalReading[n] = digitalReading[n];
  }
  MidiUSB.flush();
  delay(20);
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}
void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

