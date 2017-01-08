#ifndef ACTUATOR_HPP
#define ACTUATOR_HPP

#include "ArduMY.hpp"

/*

  Actuator is constructed from its config and its state.

  The config are all the static parameters the define its structure and features, while state is the actual dynamic value of the actuator at run time.

  The config is meant to be changed seldomly, while the state will change continuously during opperation. The serial protocol will thus optimize for quick sending of state while config is less optimized.

  Given that it will run on limited embedded hardware, the code standard of ArduMY™ will deviate from that of OctoMY™. The actuator source code is shared between ArduMY™ and OctoMY™ codebase, and thus will not rely on any Qt types or code. Both ArduMY™ & OctoMY™ may wrap this code to add convenience each in their own way.


*/

#include "ActuatorValueRepresentation.hpp"


#include "ActuatorType.hpp"
#include "ActuatorValue.hpp"

#define ACTUATOR_FLAG_SELECTOR(NAME_GET, NAME_SET, BIT)  \
	inline bool NAME_GET() const { return ( flags & (1<<(BIT)) ) > 0; } \
	inline void NAME_SET(bool val) { if(val) { flags=flags|(1<<(BIT)); } else { flags=flags&(~(1<<(BIT))); } }


// HW Actuator Descriptor for ArduMY

struct ActuatorConfig {

	ActuatorConfig()
		: flags(0)
		, type(TYPE_COUNT)
		, representation(REPRESENTATION_COUNT)
		, nickName{'\0'}
		, gearRatioNumerator(1)
		, gearRatioDenominator(1)
		, positionFeedbackPin(0)
		, tachometerPin(0)
		, incrementalEncoderPinA(0)
		, incrementalEncoderPinB(0)
		, incrementalencoderDebounceCount(0)
		, limitSwitchPinStart(0)
		, limitSwitchPinEnd(0)
		, limitSwitchDebounceCount(0)
		, stepMotorPhaseCount(4)
		, stepMotorStepsPerRotation(90)
		, rcServoPin(0)
	{

	}

	uint16_t flags;

	ACTUATOR_FLAG_SELECTOR(isLinear,				setLinear,				0 ) // Linear as opposed to rotary means that the actuator acts in a straigt motion similar to a hydraulic cylinder instead of a rotary motion similar to an RC servo
	ACTUATOR_FLAG_SELECTOR(isContinuous,			setContinuous,			1 ) // Continuous as opposed to ranged means that the actuator continues running without any stop point, as a motor instead of a servo
	ACTUATOR_FLAG_SELECTOR(hasGearRatio,			setGearRatio,			2 ) // Gear ratio means that the motor shaft may turn more than one turn per output shaft turn.
	ACTUATOR_FLAG_SELECTOR(hasPositionFeedback,		setPositionFeedback,	3 ) // Position feedback means that the actuator reports its's position from an sensor such as potensiometer to an analogue input pin
	ACTUATOR_FLAG_SELECTOR(hasTachometer,			setTachometer,			4 ) // Tachometer means that the actuator reports it's speed from a sensor such as a hall effect sensor to an digital input pin
	ACTUATOR_FLAG_SELECTOR(hasIncrementalEncoder,	setIncrementalEncoder,	5 ) // Incremental encoder means the actuator reports it's  position from an encoder via 2 digital pins. NOTE: The encoding algorithm should take gear-ratio into account
	ACTUATOR_FLAG_SELECTOR(hasAbsoluteEncoder,		setAbsoluteEncoder,		6 ) // NOTE: NOT IMPLEMENTED! Absolute encoder means the actuator reports it's  position from an encoder via X digital pins, where X usually is in the 2-16 range. NOTE: The encoding algorithm should take gear-ratio into account
	ACTUATOR_FLAG_SELECTOR(hasLimitSwitchStart,		setLimitSwitchStart,	7 ) // Limit switch start means the actuator has a limit switch telling the softare when it has reached the beginning of its range. This can be used as a security measure as well as a means for automatical reference calibration. The limit state is reported via a digital input pin
	ACTUATOR_FLAG_SELECTOR(hasLimitSwitchEnd,		setLimitSwitchEnd,		8 ) // Limit switch end means the actuator has a limit switch telling the softare when it has reached the end of its range. This can be used as a security measure as well as a means for automatical reference calibration. The limit state is reported via a digital input pin
	ACTUATOR_FLAG_SELECTOR(isDirty,					setDirty,				9 ) // Dirty means the configuration has changed and must be updated at opportunity. NOTE: This bit should be ignored when data is serialized, as it representes ephemeral state. TODO: Look at storing this outside of this class

	// The actuator type
	ActuatorType type;

	// The data type that represents the value and range for this actuator
	ActuatorValueRepresentation representation;

	// The name of this actuator in null terminated ascii / latin-1
	int8_t nickName[20];

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
		default:
		case(REPRESENTATION_COUNT): {
			//TODO: Handle this as an error somehow
		} break;
		}
		return out;
	}

	void clear()
	{
		flags=0;
		type=TYPE_COUNT;
		representation=REPRESENTATION_COUNT;
		for(int i=0; i<20; ++i) {
			nickName[i]='\0';
		}
		gearRatioNumerator=1;
		gearRatioDenominator=1;
		positionFeedbackPin=0;
		tachometerPin=0;
		incrementalEncoderPinA=0;
		incrementalEncoderPinB=0;
		incrementalencoderDebounceCount=0;
		limitSwitchPinStart=0;
		limitSwitchPinEnd=0;
		limitSwitchDebounceCount=0;
		stepMotorPhaseCount=4;
		stepMotorStepsPerRotation=90;
		rcServoPin=0;
		rangeStart.quadWord=0;
		rangeSpan.quadWord=0;
	}

	bool isEqual(const ActuatorConfig &other) const
	{
		// Mask to remove ephemeral parts of flags (currently only isDirty)
		const uint16_t mask=flags&(~(1<<9));
		if( (flags & mask) != (other.flags&mask)) {
			return false;
		}
		if(type!=other.type) {
			return false;
		}
		// TODO: factorize to find "different but same" like 2:2, 3:3 etc.
		if(hasGearRatio() && (gearRatioNumerator != other.gearRatioNumerator) ) {
			return false;
		}
		if(hasGearRatio() && (gearRatioDenominator!= other.gearRatioDenominator) ) {
			return false;
		}
		if(hasPositionFeedback() && (positionFeedbackPin != other.positionFeedbackPin)) {
			return false;
		}
		if(hasTachometer() && (tachometerPin != other.tachometerPin)) {
			return false;
		}
		if(hasIncrementalEncoder() && (incrementalEncoderPinA != other.incrementalEncoderPinA)) {
			return false;
		}
		if(hasIncrementalEncoder() && (incrementalEncoderPinB != other.incrementalEncoderPinB)) {
			return false;
		}
		if(hasIncrementalEncoder() && (incrementalencoderDebounceCount != other.incrementalencoderDebounceCount)) {
			return false;
		}
		if(hasLimitSwitchStart() && (limitSwitchPinStart != other.limitSwitchPinStart)) {
			return false;
		}
		if(hasLimitSwitchEnd() && (limitSwitchPinEnd != other.limitSwitchPinEnd)) {
			return false;
		}
		if((hasLimitSwitchStart() || hasLimitSwitchEnd())  && (limitSwitchDebounceCount != other.limitSwitchDebounceCount)) {
			return false;
		}

		switch(type) {

		case(STEP_MOTOR): {
			if(stepMotorPhaseCount!= other.stepMotorPhaseCount) {
				return false;
			}
			if(stepMotorStepsPerRotation!= other.stepMotorStepsPerRotation) {
				return false;
			}
		}
		break;

		case(RC_SERVO): {
			if(rcServoPin!= other.rcServoPin) {
				return false;
			}
		}
		break;
		case(DC_MOTOR):
		case(RELAY):
		case(TYPE_COUNT):
		default:
			break;

		}


		switch(representation) {
		case(ActuatorValueRepresentation::BIT): {
			if(rangeStart.bit != other.rangeStart.bit) {
				return false;
			}
			if(rangeSpan.bit != other.rangeSpan.bit) {
				return false;
			}
		}
		break;
		case(ActuatorValueRepresentation::BYTE): {
			if(rangeStart.byte != other.rangeStart.byte) {
				return false;
			}
			if(rangeSpan.byte != other.rangeSpan.byte) {
				return false;
			}
		}
		break;
		case(ActuatorValueRepresentation::WORD): {
			if(rangeStart.word != other.rangeStart.word) {
				return false;
			}
			if(rangeSpan.word != other.rangeSpan.word) {
				return false;
			}
		}
		break;
		case(ActuatorValueRepresentation::DOUBLE_WORD): {
			if(rangeStart.doubleWord != other.rangeStart.doubleWord) {
				return false;
			}
			if(rangeSpan.doubleWord != other.rangeSpan.doubleWord) {
				return false;
			}
		}
		break;
		case(ActuatorValueRepresentation::QUAD_WORD): {
			if(rangeStart.quadWord != other.rangeStart.quadWord) {
				return false;
			}
			if(rangeSpan.quadWord != other.rangeSpan.quadWord) {
				return false;
			}
		}
		break;
		case(ActuatorValueRepresentation::SINGLE_FLOAT): {
			if(rangeStart.singlePrecision != other.rangeStart.singlePrecision) {
				return false;
			}
			if(rangeSpan.singlePrecision != other.rangeSpan.singlePrecision) {
				return false;
			}
		}
		break;
		case(ActuatorValueRepresentation::DOUBLE_FLOAT): {
			if(rangeStart.doublePrecision != other.rangeStart.doublePrecision) {
				return false;
			}
			if(rangeSpan.doublePrecision != other.rangeSpan.doublePrecision) {
				return false;
			}
		}
		break;
		default:
		case(ActuatorValueRepresentation::REPRESENTATION_COUNT): {
			//Ignore, as this is not an error at comparison time
		}
		break;
		}

		// By not failig until now we win!
		return true;
	}

};

bool operator== (const ActuatorConfig &a, const ActuatorConfig &b);


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
