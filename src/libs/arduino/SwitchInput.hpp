#ifndef SWITCHINPUT_HPP
#define SWITCHINPUT_HPP

#include "ArduMY.hpp"


struct SwitchInput {
  uint8_t pin;
  uint32_t debounceTime;
  bool isPulseSwitch;
  bool lastValue;
  bool debouncedValue;
  uint32_t lastTime;


  SwitchInput(
	uint8_t pin
	, uint32_t debounceTime
	, bool isPulseSwitch = false
  )
	: pin( pin )
	, debounceTime( debounceTime )
	, isPulseSwitch( isPulseSwitch )
	, lastValue( false )
	, debouncedValue( false )
	, lastTime(0)
  {
  }

  void setup() {
	/*
	  Serial.print(isPulseSwitch ? "Pulse" : "Hold");
	  Serial.print(" switch @ pin ");
	  Serial.print(pin);
	  Serial.print(" with ");
	  Serial.print(debounceTime);
	  Serial.println("ms debounce time");
	*/
	pinMode(pin, INPUT_PULLUP);
  }


  bool update(uint32_t now) {
	bool didPulse = false;
	int state = digitalRead(pin);
	//Serial.println(state);
	const bool value = (1 == state);
	if (value != lastValue) {
	  lastTime = now;
	  //Serial.print("BUTTON ");      Serial.print(pin);      Serial.print(": Changed to ");      Serial.println(value);
	}
	const auto timeSinceLastChange = now - lastTime;
	if ( timeSinceLastChange > debounceTime ) {
	  if (value != debouncedValue) {
		didPulse = debouncedValue;
		//Serial.print(pin); Serial.print(", "); Serial.print(lastValue); Serial.print(", "); debouncedValue? Serial.println(":ON, "): Serial.println(":OFF, ");
		debouncedValue = value;
	  }
	}
	lastValue = value;
	if (isPulseSwitch) {
	  //Serial.println("PULSERET");
	  return didPulse;
	}
	else {
	  //Serial.println("HOLDRET");
	  return !debouncedValue;// pullup resistor inverts value so we invert it back!
	}
  }

};

#endif // SWITCHINPUT_HPP
