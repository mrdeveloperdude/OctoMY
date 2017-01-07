#ifndef ACTUATOR_HPP
#define ACTUATOR_HPP

#include "ArduMY.hpp"

/*

  Actuator is constructed from its config and its state.

  The config are all the static parameters the define its structure and features, while state is the actual dynamic value of the actuator at run time.

  The config is meant to be changed seldomly, while the state will change continuously during opperation. The serial protocol will thus optimize for quick sending of state while config is less optimized.

  Given that it will run on limited embedded hardware, the code standard of ArduMY™ will deviate from that of OctoMY™. The actuator source code is shared between ArduMY™ and OctoMY™ codebase, and thus will not rely on any Qt types or code. Both ArduMY™ & OctoMY™ may wrap this code to add convenience each in their own way.


*/

enum ActuatorRepresentation: uint8_t {
	BIT
	, BYTE
	, WORD
	, DOUBLE_WORD
	, QUAD_WORD
	, SINGLE_FLOAT
	, DOUBLE_FLOAT
	, REPRESENTATION_COUNT
};

enum ActuatorType: uint8_t {
	DC_MOTOR, STEP_MOTOR, RC_SERVO, RELAY, TYPE_COUNT
};

// This may waste a little memory, but I think the flexibliity and convenience it adds makes it worth it
union ActuatorValue {
	// The actual position for actuator. Bound by values of rangeStart and rangeSpan in config for all except bit.
	// + Interpreted as absolute position in non-continuous actuator
	// + Interpreted as accelleration for continuous actuator
	// + Interpreted as step in stepped actuator
	bool bit;				// 1 - bit int (boolean)
	uint8_t  byte;			// 8 - bit int
	uint16_t word;			// 16 - bit int
	uint32_t doubleWord;	// 32 - bit int
	uint64_t quadWord;		// 64 - bit int
	float singlePrecision;
	double doublePrecision;
};

#define ACTUATOR_FLAG_SELECTOR(NAME_GET, NAME_SET, BIT)  \
	inline bool NAME_GET() const { return ( flags & (1<<(BIT)) ) > 0; } \
	inline void NAME_SET(bool val) { if(val) { flags=flags|(1<<(BIT)); } else { flags=flags&(~(1<<(BIT))); } }


// HW Actuator Descriptor for ArduMY

struct ActuatorConfig {

	uint16_t flags;

	ACTUATOR_FLAG_SELECTOR(isLinear,				setLinear,				0 ) // Linear as opposed to rotary means that the actuator acts in a straigt motion similar to a hydraulic cylinder instead of a rotary motion similar to an RC servo
	ACTUATOR_FLAG_SELECTOR(isContinuous,			setContinuous,			1 ) // Continuous as opposed to ranged means that the actuator continues running without any stop point, as a motor instead of a servo
	ACTUATOR_FLAG_SELECTOR(hasGearRatio,			setGearRatio,			2 ) // Gear ratio means that the motor shaft may turn more than one turn per output shaft turn.
	ACTUATOR_FLAG_SELECTOR(hasPositionFeedback,		setPositionFeedback,	3 ) // Position feedback means that the actuator reports its's position from an sensor such as potensiometer to an analogue input pin
	ACTUATOR_FLAG_SELECTOR(hasTachometer,			setTachometer,			4 ) // Tachometer means that the actuator reports it's speed from a sensor such as a hall effect sensor to an digital input pin
	ACTUATOR_FLAG_SELECTOR(hasIncrementalEncoder,	setIncrementalEncoder,	5 ) // Incremental encoder means the actuatorreports it's  position from an encoder via 2 digital pins. NOTE: The encoding algorithm should take gear-ratio into account
	ACTUATOR_FLAG_SELECTOR(hasAbsoluteEncoder,		setAbsoluteEncoder,		6 ) // Absolute encoder means the actuatorreports it's  position from an encoder via X digital pins, where X usually is in the 2-16 range. NOTE: The encoding algorithm should take gear-ratio into account
	ACTUATOR_FLAG_SELECTOR(hasLimitSwitchStart,		setLimitSwitchStart,	7 ) // Limit switch start means the actuator has a limit switch telling the softare when it has reached the beginning of its range. This can be used as a security measure as well as a means for automatical reference calibration. The limit state is reported via a digital input pin
	ACTUATOR_FLAG_SELECTOR(hasLimitSwitchEnd,		setLimitSwitchEnd,		8 ) // Limit switch end means the actuator has a limit switch telling the softare when it has reached the end of its range. This can be used as a security measure as well as a means for automatical reference calibration. The limit state is reported via a digital input pin
	ACTUATOR_FLAG_SELECTOR(isDirty,					setDirty,				9 ) // The configuration has changed and must be updated at opportunity

	// The actuator type
	ActuatorType type;

	// The data type that represents the value and range for this actuator
	ActuatorRepresentation representation;

	// The name of this actuator in null terminated ascii / latin-1
	char nickName[20];

	//The gear ratio.
	uint8_t gearRatioNumerator, gearRatioDenominator;

	// The hardware analogue in pin number for position feedback
	// TODO: Decide how to calibrate and use the value returned by this pin
	uint8_t positionFeedbackPin;

	// The hardware digital in pin for tachometer
	// TODO: Decide how to calibrate and use the value returned by this pin
	uint8_t tachometerPin;

	// The hardware digital in pins (A & B) for incremental encoder
	uint8_t incrementalEncoderPinA;
	uint8_t incrementalEncoderPinB;
	// The number of iterations of stable input must pass before a change in value is accepted
	uint8_t incrementalencoderDebounceCount;

	// The hardware digital in pins (start & end) for limit switches
	uint8_t limitSwitchPinStart;
	uint8_t limitSwitchPinEnd;
	uint8_t limitSwitchDebounceCount;

	// The number of windings in the stepmotor
	uint8_t stepMotorPhaseCount;
	// The number of steps to a rotation. NOTE: This is BEFORE any gear ratio is included in the calculation
	uint16_t stepMotorStepsPerRotation;

	// The hardware digital in pin for sending PWM to RC servo
	uint8_t rcServoPin;

	// The start + range trim for actuator
	ActuatorValue rangeStart;
	ActuatorValue rangeSpan;

	ActuatorValue rangeLow()
	{
		return rangeStart;
	}
	ActuatorValue rangeHigh()
	{
		ActuatorValue out=rangeStart;
		switch(representation) {
		case(BIT): {
			out.bit=rangeStart.bit + rangeSpan.bit;
		}
		break;
		case(BYTE): {
			out.byte=rangeStart.byte + rangeSpan.byte;
		}
		break;
		case(WORD): {
			out.word=rangeStart.word + rangeSpan.word;
		}
		break;
		case(DOUBLE_WORD): {
			out.doubleWord=rangeStart.doubleWord + rangeSpan.doubleWord;
		}
		break;
		case(QUAD_WORD): {
			out.quadWord=rangeStart.quadWord + rangeSpan.quadWord;
		}
		break;
		case(SINGLE_FLOAT): {
			out.singlePrecision=rangeStart.singlePrecision + rangeSpan.singlePrecision;
		}
		break;
		case(DOUBLE_FLOAT): {
			out.doublePrecision=rangeStart.doublePrecision + rangeSpan.doublePrecision;
		}
		break;
		}
		return out;
	}

};



struct ActuatorState {


	unsigned int flags;

	ACTUATOR_FLAG_SELECTOR(isLimp,					setLimp,				0 )
	ACTUATOR_FLAG_SELECTOR(isDirty,					setDirty,				1 )

	ActuatorValue value;
};

struct Actuator {
	ActuatorConfig config;
	ActuatorState state;
};


#undef ACTUATOR_FLAG_SELECTOR

#endif // ACTUATOR_HPP
