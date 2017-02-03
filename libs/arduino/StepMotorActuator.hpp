#ifndef STEPMOTORACTUATOR_HPP
#define STEPMOTORACTUATOR_HPP

#include "ArduMY.hpp"


#include <AccelStepper.h>
#include <MultiStepper.h>

// From http://forum.arduino.cc/index.php?topic=71964.15
#define SPR (4097)
//(25792.0f/405.0f )



struct StepMotorPins {
	uint8_t pinA;
	uint8_t pinB;
	uint8_t pinC;
	uint8_t pinD;

	StepMotorPins(
		uint8_t pinA
		, uint8_t pinB
		, uint8_t pinC
		, uint8_t pinD
	)
		: pinA(pinA)
		, pinB(pinB)
		, pinC(pinC)
		, pinD(pinD)
	{
	}
};


struct StepMotorActuator {
	AccelStepper stepper;
	StepMotorPins pins;
	float stepsPerRevolution;
	float speed;
	uint32_t lastStepTime;
	bool isRunning;


	StepMotorActuator(
		float stepsPerRevolution
		, StepMotorPins pins
		, float speed
	)
		: stepper( AccelStepper::FULL4WIRE, pins.pinA, pins.pinC, pins.pinB, pins.pinD )
		, pins( pins )
		, stepsPerRevolution( stepsPerRevolution )
		, speed( speed )
		, isRunning(false)
	{
		stepper.setSpeed(0.0);
		stepper.setMaxSpeed((speed * stepsPerRevolution) / 60.0);
		stepper.disableOutputs();
	}


	void trigger(uint32_t now)
	{
		isRunning = true;
		stepper.enableOutputs();
	}


	void setup()
	{

	}

	void update(uint32_t now)
	{
		uint32_t a = now - startTime;
		//Serial.println(a);
		float val=0.0f;
		if (val > 0.0001f) {
			const float maxStepsPerSec = stepper.maxSpeed();
			const float stepsPerSec = ((maxStepsPerSec) * val);
			//Serial.print("stepsPerSec: ");  Serial.println(stepsPerSec);
			stepper.setSpeed(stepsPerSec);
		} else {
			stepper.setSpeed(0.0);
			stepper.disableOutputs();
		}
		stepper.runSpeed();
	}
};


#endif // STEPMOTORACTUATOR_HPP
