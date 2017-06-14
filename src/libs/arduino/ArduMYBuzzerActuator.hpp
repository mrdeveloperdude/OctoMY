#ifndef BUZZERACTUATOR_HPP
#define BUZZERACTUATOR_HPP

#include "ArduMY.hpp"

struct BuzzerActuator {

  uint8_t pin;

  int32_t state = LOW;
  int32_t last = 0;
  uint32_t playStartTime = 0;
  float  step = 1.0f;
  int lastNoteIndex;
  bool isPlaying = false;

  struct Note {
	uint16_t hz; //Hertz for the tone
	int16_t ms; // Positive is play, negative is pause in milliseconds
  };
  Note notes[32];

  BuzzerActuator(uint8_t pin):
	pin(pin)
  {

  }

  void setup() {
	pinMode(pin, OUTPUT);
  }

  void play(uint32_t now, Note (&n)[32]) {
	//Serial.println("---");    Serial.println("---");    Serial.println("---------------------------- BUSSER PLAY");

	for (int i = 0; i < 32; ++i) {
	  notes[i] = n[i];
	}
	lastNoteIndex = -1;
	playStartTime = now;
	state = LOW;
	last = 0;
	step = 1.0f;
	isPlaying = true;
  }

  static inline bool fuzzyCompare(float p1, float p2)
  {
	return (fabs(p1 - p2) * 100000.f <= min(fabs(p1), fabs(p2)));
  }

  void update(uint32_t now) {
	if (isPlaying) {
	  const uint32_t playedFor = now - playStartTime;
	  uint32_t acc = 0;
	  int i = 0;
	  bool found = false;
	  for (; i < 32; ++i) {
		const Note &note = notes[i];
		const int16_t ms = ( (note.ms < 0) ? (-note.ms) : (note.ms) );
		// End song on first encountered 0 (<0 is error)
		if (ms <= 0) {
		  if (ms < 0) {
			// Serial.println("NOTE MS ERROR!!!!!");
		  }
		  isPlaying = false;
		  digitalWrite(pin, LOW);
		  return;
		}
		acc += ms;
		if (acc > playedFor) {
		  found = true;
		  break;
		}
	  }

	  if (found) {
		const Note &note = notes[i];
		if ( lastNoteIndex != i)  {
		  //Serial.print("NEW NOTE #"); Serial.print(i); Serial.print(" / 32: now=");    Serial.print(now);      Serial.print(", for=");    Serial.print(playedFor);      Serial.print(", ");     Serial.print(note.hz);      Serial.print(" DHz @ ");      Serial.println(note.ms);
		  lastNoteIndex = i;
		  const float hz = ((float)note.hz) / 10.0f;
		  step = 1000000.0f / ((hz > 0.0f) ? hz : 1.0f);
		}
		if (note.ms > 0) {
		  auto const microNow = micros();
		  const float since = ((float)microNow - last);
		  if (since > step) {
			state = !state;
			digitalWrite(pin, state);
			last = microNow;
		  }
		}
	  }
	}
  }
};

#endif // BUZZERACTUATOR_HPP
