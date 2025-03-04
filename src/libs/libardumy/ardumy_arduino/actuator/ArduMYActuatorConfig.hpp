#ifndef ACTUATORCONFIG_HPP
#define ACTUATORCONFIG_HPP


#include "ardumy_arduino/ArduMY.hpp"

#include "ardumy_arduino/actuator/ArduMYActuatorFlags.hpp"
#include "ardumy_arduino/actuator/ArduMYActuatorType.hpp"
#include "ardumy_arduino/actuator/ArduMYActuatorValue.hpp"
#include "ardumy_arduino/actuator/ArduMYActuatorValueRepresentation.hpp"

// HW Actuator Descriptor for ArduMY
struct ArduMYActuatorConfig {

	ArduMYActuatorConfig()
		: flags(0)
		, type(TYPE_COUNT)
		, representation(VALREP_REPRESENTATION_COUNT)
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

	ARDUMY_ACTUATOR_FLAG_SELECTOR(isLinear,              setLinear,             0 ) // Linear as opposed to rotary means that the actuator acts in a straigt motion similar to a hydraulic cylinder instead of a rotary motion similar to an RC servo
	ARDUMY_ACTUATOR_FLAG_SELECTOR(isContinuous,          setContinuous,         1 ) // Continuous as opposed to ranged means that the actuator continues running without any stop point, as a motor instead of a servo
	ARDUMY_ACTUATOR_FLAG_SELECTOR(hasGearRatio,          setGearRatio,          2 ) // Gear ratio means that the motor shaft may turn more than one turn per output shaft turn.
	ARDUMY_ACTUATOR_FLAG_SELECTOR(hasPositionFeedback,   setPositionFeedback,	3 ) // Position feedback means that the actuator reports its's position from an sensor such as potensiometer to an analogue input pin
	ARDUMY_ACTUATOR_FLAG_SELECTOR(hasTachometer,         setTachometer,         4 ) // Tachometer means that the actuator reports its speed from a sensor such as a hall effect sensor to an digital input pin
	ARDUMY_ACTUATOR_FLAG_SELECTOR(hasIncrementalEncoder, setIncrementalEncoder, 5 ) // Incremental encoder means the actuator reports its  position from an encoder via 2 digital pins. NOTE: The encoding algorithm should take gear-ratio into account
	ARDUMY_ACTUATOR_FLAG_SELECTOR(hasAbsoluteEncoder,    setAbsoluteEncoder,    6 ) // NOTE: NOT IMPLEMENTED! Absolute encoder means the actuator reports its  position from an encoder via X digital pins, where X usually is in the 2-16 range. NOTE: The encoding algorithm should take gear-ratio into account
	ARDUMY_ACTUATOR_FLAG_SELECTOR(hasLimitSwitchStart,   setLimitSwitchStart,   7 ) // Limit switch start means the actuator has a limit switch telling the softare when it has reached the beginning of its range. This can be used as a security measure as well as a means for automatical reference calibration. The limit state is reported via a digital input pin
	ARDUMY_ACTUATOR_FLAG_SELECTOR(hasLimitSwitchEnd,     setLimitSwitchEnd,     8 ) // Limit switch end means the actuator has a limit switch telling the softare when it has reached the end of its range. This can be used as a security measure as well as a means for automatical reference calibration. The limit state is reported via a digital input pin
	ARDUMY_ACTUATOR_FLAG_SELECTOR(isDirty,               setDirty,              9 ) // Dirty means the configuration has changed and must be updated at opportunity. NOTE: This bit should be ignored when data is serialized, as it representes ephemeral state. TODO: Look at storing this outside of this class

	// The actuator type
	ArduMYActuatorType type;

	// The data type that represents the value and range for this actuator
	ArduMYActuatorValueRepresentation representation;

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
	ArduMYActuatorValue rangeStart;
	ArduMYActuatorValue rangeSpan;

	ArduMYActuatorValue rangeLow()
	{
		return rangeStart;
	}
	ArduMYActuatorValue rangeHigh()
	{
		ArduMYActuatorValue out=rangeStart;
		switch(representation) {
		case(VALREP_BIT): {
			out.bit=rangeStart.bit + rangeSpan.bit;
		}
		break;
		case(VALREP_BYTE): {
			out.byte=rangeStart.byte + rangeSpan.byte;
		}
		break;
		case(VALREP_WORD): {
			out.word=rangeStart.word + rangeSpan.word;
		}
		break;
		case(VALREP_DOUBLE_WORD): {
			out.doubleWord=rangeStart.doubleWord + rangeSpan.doubleWord;
		}
		break;
		case(VALREP_QUAD_WORD): {
			out.quadWord=rangeStart.quadWord + rangeSpan.quadWord;
		}
		break;
		case(VALREP_SINGLE_FLOAT): {
			out.singlePrecision=rangeStart.singlePrecision + rangeSpan.singlePrecision;
		}
		break;
		case(VALREP_DOUBLE_FLOAT): {
			out.doublePrecision=rangeStart.doublePrecision + rangeSpan.doublePrecision;
		}
		break;
		default:
		case(VALREP_REPRESENTATION_COUNT): {
			//TODO: Handle this as an error somehow
		} break;
		}
		return out;
	}

	void clear()
	{
		flags=0;
		type=TYPE_COUNT;
		representation=VALREP_REPRESENTATION_COUNT;
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

	bool isEqual(const ArduMYActuatorConfig &other) const
	{
		// Mask to remove ephemeral parts of flags (currently only isDirty)
		const uint16_t temp=(1<<9);
		const uint16_t mask=(~temp);

		const uint16_t maskedFlags=(flags & mask);
		const uint16_t otherMaskedFlags=(other.flags & mask);

		if( maskedFlags != otherMaskedFlags) {
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

		if(!rangeStart.isEqual(other.rangeStart,representation)) {
			return false;
		}
		if(!rangeSpan.isEqual(other.rangeSpan,representation)) {
			return false;
		}

		// By not failig until now we win!
		return true;
	}

};


bool operator== (const ArduMYActuatorConfig &a, const ArduMYActuatorConfig &b);
bool operator!= (const ArduMYActuatorConfig &a, const ArduMYActuatorConfig &b);



#endif
// ACTUATORCONFIG_HPP
