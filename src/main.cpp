#include <Arduino.h>

// DRV8825 wiring: see agents/stepper-context.md
// Motor (17HE15-1504S): BLK=A+ -> 1A, BLU=A- -> 1B, GRN=B+ -> 2A, RED=B- -> 2B
// Stepper music: step rate (steps/sec) = pitch (Hz).
const int stepPin = 2; // swapped: breadboard wiring has pin 2 on STP
const int dirPin = 3;

const unsigned int eighthMs = 200; // tempo: one eighth note; lower = faster song

// Note frequencies in Hz (0 = rest)
const unsigned int REST = 0;
const unsigned int NOTE_A3 = 220;
const unsigned int NOTE_B3 = 247;
const unsigned int NOTE_C4 = 262;
const unsigned int NOTE_D4 = 294;
const unsigned int NOTE_E4 = 330;
const unsigned int NOTE_F4 = 349;
const unsigned int NOTE_G4 = 392;
const unsigned int NOTE_A4 = 440;

struct Note {
  unsigned int freq;
  byte eighths; // length in eighth notes
};

// Korobeiniki (the "Tetris theme"), traditional
const Note song[] = {
  {NOTE_E4, 2}, {NOTE_B3, 1}, {NOTE_C4, 1}, {NOTE_D4, 2}, {NOTE_C4, 1}, {NOTE_B3, 1},
  {NOTE_A3, 2}, {NOTE_A3, 1}, {NOTE_C4, 1}, {NOTE_E4, 2}, {NOTE_D4, 1}, {NOTE_C4, 1},
  {NOTE_B3, 3}, {NOTE_C4, 1}, {NOTE_D4, 2}, {NOTE_E4, 2},
  {NOTE_C4, 2}, {NOTE_A3, 2}, {NOTE_A3, 2}, {REST, 2},

  {REST, 1}, {NOTE_D4, 2}, {NOTE_F4, 1}, {NOTE_A4, 2}, {NOTE_G4, 1}, {NOTE_F4, 1},
  {NOTE_E4, 3}, {NOTE_C4, 1}, {NOTE_E4, 2}, {NOTE_D4, 1}, {NOTE_C4, 1},
  {NOTE_B3, 2}, {NOTE_B3, 1}, {NOTE_C4, 1}, {NOTE_D4, 2}, {NOTE_E4, 2},
  {NOTE_C4, 2}, {NOTE_A3, 2}, {NOTE_A3, 2}, {REST, 2},
};
const int songLength = sizeof(song) / sizeof(song[0]);

bool forward = true;

void playNote(unsigned int freq, unsigned long durationMs) {
  // Short gap at the end of each note so repeated notes sound separate
  unsigned long gapMs = durationMs / 10;
  unsigned long soundMs = durationMs - gapMs;

  if (freq == REST) {
    delay(durationMs);
    return;
  }

  // Flip direction every note so the shaft stays roughly in place
  forward = !forward;
  digitalWrite(dirPin, forward ? HIGH : LOW);

  unsigned long periodUs = 1000000UL / freq;
  unsigned long steps = (unsigned long)freq * soundMs / 1000;
  for (unsigned long i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(periodUs - 10);
  }
  delay(gapMs);
}

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("stepper music");
}

void loop() {
  for (int i = 0; i < songLength; i++) {
    playNote(song[i].freq, (unsigned long)song[i].eighths * eighthMs);
  }
  delay(2000);
}
