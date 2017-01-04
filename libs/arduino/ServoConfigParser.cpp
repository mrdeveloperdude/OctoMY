#include "ServoConfigParser.hpp"

#include "CommandParser.hpp"


ServoConfigParser::ServoConfigParser(CommandParser &commandParser)
	: commandParser(commandParser)
	, step(ACTUATOR_INDEX)
	, byteIndex(0)
	, actuatorIndex(0)
{
	reset();
}

void ServoConfigParser::reset()
{
	step=ACTUATOR_INDEX;
	byteIndex=0;
	actuatorIndex=0;
}


/*
 * uint16_t flags;

	ACTUATOR_FLAG_SELECTOR(isLinear,				setLinear,				0 ) // Linear as opposed to rotary means that the actuator acts in a straigt motion similar to a hydraulic cylinder instead of a rotary motion similar to an RC servo
	ACTUATOR_FLAG_SELECTOR(isContinuous,			setContinuous,			1 ) // Continuous as opposed to ranged means that the actuator continues running without any stop point, as a motor instead of a servo
	//ACTUATOR_FLAG_SELECTOR(isStepped,				setStepped,				2 ) // Stepped as opposed to smooth means that actuator is one of many pre-set steps. The number of steps is defined by valueRange
	ACTUATOR_FLAG_SELECTOR(hasGearRatio,			setGearRatio,			3 ) // Gear ratio means that the motor shaft may turn more than one turn per output shaft turn.
	ACTUATOR_FLAG_SELECTOR(hasPositionFeedback,		setPositionFeedback,	4 ) // Position feedback means that the actuator reports its's position from an sensor such as potensiometer to an analogue input pin
	ACTUATOR_FLAG_SELECTOR(hasTachometer,			setTachometer,			5 ) // Tachometer means that the actuator reports it's speed from a sensor such as a hall effect sensor to an digital input pin
	ACTUATOR_FLAG_SELECTOR(hasIncrementalEncoder,	setIncrementalEncoder,	6 ) // Incremental encoder means the actuatorreports it's  position from an encoder via 2 digital pins. NOTE: The encoding algorithm should take gear-ratio into account
	ACTUATOR_FLAG_SELECTOR(hasAbsoluteEncoder,		setAbsoluteEncoder,		7 ) // Absolute encoder means the actuatorreports it's  position from an encoder via X digital pins, where X usually is in the 2-16 range. NOTE: The encoding algorithm should take gear-ratio into account
	ACTUATOR_FLAG_SELECTOR(hasLimitSwitchStart,		setLimitSwitchStart,	8 ) // Limit switch start means the actuator has a limit switch telling the softare when it has reached the beginning of its range. This can be used as a security measure as well as a means for automatical reference calibration. The limit state is reported via a digital input pin
	ACTUATOR_FLAG_SELECTOR(hasLimitSwitchEnd,		setLimitSwitchEnd,		9 ) // Limit switch end means the actuator has a limit switch telling the softare when it has reached the end of its range. This can be used as a security measure as well as a means for automatical reference calibration. The limit state is reported via a digital input pin
	ACTUATOR_FLAG_SELECTOR(hasRCServoInterface,		setRCServoInterface,	10) // This actuator has an RC style interface, where the position is controlled via PWM with duty cycle between 1000 and 2000 microseconds.
	ACTUATOR_FLAG_SELECTOR(isDirty,					setDirty,				11) // The configuration has changed and must be updated at opportunity

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

*/


ActuatorConfig &ServoConfigParser::currentConfig()
{
	return commandParser.actuators[actuatorIndex].config;
}

// Look at flags and go from the current step to the next enabled one
void ServoConfigParser::nextStep()
{
	ActuatorConfig &act=currentConfig();
	switch(step) {
	case(ACTUATOR_INDEX): {
		step=FLAGS;
	}
	break;
	case(FLAGS): {
		step=TYPE;
	}
	break;
	case(TYPE): {
		step=REPRESENTATION;
	}
	break;
	case(REPRESENTATION): {
		step=NICK;
	}
	break;
	case(NICK): {
		if(act.hasGearRatio()) {
			step=GEAR_NUMERATOR;
		} else {
			step=GEAR_DENOMINATOR;
			nextStep();
		}
	}
	break;
	case(GEAR_NUMERATOR): {
		step=GEAR_DENOMINATOR;
	}
	break;
	case(GEAR_DENOMINATOR): {
		step=POSITION_FEEDBACK;
		if(!act.hasPositionFeedback()) {
			nextStep();
		}
	}
	break;
	case(POSITION_FEEDBACK): {
		step=TACHOMETER;
		if(!act.hasTachometer()) {
			nextStep();
		}
	}
	break;
	case(TACHOMETER): {
		if(act.hasIncrementalEncoder()) {
			step=ENCODER_PIN_A;
		} else {
			step=ENCODER_DEBOUNCE;
			nextStep();
		}
	}
	break;
	case(ENCODER_PIN_A): {
		step=ENCODER_PIN_B;
	}
	break;
	case(ENCODER_PIN_B): {
		step=ENCODER_DEBOUNCE;
	}
	break;
	case(ENCODER_DEBOUNCE): {
		step=LIMIT_SWITCH_PIN_START;
		if(!act.hasLimitSwitchStart()) {
			nextStep();
		}
	}
	break;
	case(LIMIT_SWITCH_PIN_START): {
		step=LIMIT_SWITCH_PIN_END;
		if(!act.hasLimitSwitchEnd()) {
			nextStep();
		}
	}
	break;
	case(LIMIT_SWITCH_PIN_END): {
		step=LIMIT_SWITCH_DEBOUNCE;
		if((!act.hasLimitSwitchStart()) && (!act.hasLimitSwitchEnd() )) {
			nextStep();
		}
	}
	break;
	case(LIMIT_SWITCH_DEBOUNCE): {
		if(STEP_MOTOR==act.type) {
			step=STEP_PHASE_COUNT;
		} else {
			step=STEP_STEPS_PER_ROTATION;
			nextStep();
		}
	}
	break;
	case(STEP_PHASE_COUNT): {
		step=STEP_STEPS_PER_ROTATION;
	}
	break;
	case(STEP_STEPS_PER_ROTATION): {

		if(RC_SERVO==act.type) {
			step=RC_SERVO_PIN;
		} else {
			step=END_OF_PARSE;
		}
	}
	break;
	case(RC_SERVO_PIN): {
		step=END_OF_PARSE;
	}
	break;

	case(END_OF_PARSE): {
		//Do nothing. Maybe this is an indication of error, and should be reported?
	}
	break;
	}

	byteIndex=0;
}

bool ServoConfigParser::parse(const uint8_t in)
{
	switch(step) {
	case(ACTUATOR_INDEX): {
		actuatorIndex=in;
		nextStep();
	}
	break;
	case(FLAGS): {
		if(byteIndex==2) {
			currentConfig().flags=converter.uint16;
			nextStep();
		} else {
			converter.uint8[byteIndex]=in;
			byteIndex++;
		}
	}
	break;
	case(NICK): {
		if(byteIndex==20) {
			nextStep();
		} else {
			currentConfig().nickName[byteIndex]=(char)in;
			byteIndex++;
		}
	}
	break;

	case(GEAR_NUMERATOR): {
		currentConfig().gearRatioNumerator=in;
		nextStep();
	}
	break;


	case(GEAR_DENOMINATOR): {
		currentConfig().gearRatioDenominator=in;
		nextStep();
	}
	break;

	case(POSITION_FEEDBACK): {
		currentConfig().positionFeedbackPin=in;
		nextStep();
	}
	break;

	case(TACHOMETER): {
		currentConfig().tachometerPin=in;
		nextStep();
	}
	break;

	case(ENCODER_PIN_A): {
		currentConfig().incrementalEncoderPinA=in;
		nextStep();
	}
	break;

	case(ENCODER_PIN_B): {
		currentConfig().incrementalEncoderPinB=in;
		nextStep();
	}
	break;

	case(ENCODER_DEBOUNCE): {
		currentConfig().incrementalencoderDebounceCount=in;
		nextStep();
	}
	break;

	case(LIMIT_SWITCH_PIN_START): {
		currentConfig().limitSwitchPinStart=in;
		nextStep();
	}
	break;

	case(LIMIT_SWITCH_PIN_END): {
		currentConfig().limitSwitchPinEnd=in;
		nextStep();
	}
	break;

	case(LIMIT_SWITCH_DEBOUNCE): {
		currentConfig().limitSwitchDebounceCount=in;
		nextStep();
	}
	break;

	case(STEP_PHASE_COUNT): {
		currentConfig().stepMotorPhaseCount=in;
		nextStep();
	}
	break;

	case(STEP_STEPS_PER_ROTATION): {
		if(byteIndex==2) {
			currentConfig().stepMotorStepsPerRotation=converter.uint16;
			nextStep();
		} else {
			converter.uint8[byteIndex]=in;
			byteIndex++;
		}
	}
	break;

	case(RC_SERVO_PIN): {
		currentConfig().rcServoPin=in;
		nextStep();
	}
	break;


	}

	return (END_OF_PARSE==step);
}
