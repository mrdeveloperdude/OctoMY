#include "ActuatorConfigSerializerBase.hpp"

ActuatorConfigSerializerBase::ActuatorConfigSerializerBase()
	: config(nullptr)
	, step(END_OF_OPERATION)
	, byteIndex(0)
{

}


void ActuatorConfigSerializerBase::setConfig(ActuatorConfig &c)
{
	config=&c;
	reset();
}


void ActuatorConfigSerializerBase::reset()
{
	step=FLAGS;
	byteIndex=0;
}



// Look at flags & type and go from the current step to the next enabled one
void ActuatorConfigSerializerBase::nextStep()
{
	switch(step) {
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
		if(config->hasGearRatio()) {
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
		if(!config->hasPositionFeedback()) {
			nextStep();
		}
	}
	break;
	case(POSITION_FEEDBACK): {
		step=TACHOMETER;
		if(!config->hasTachometer()) {
			nextStep();
		}
	}
	break;
	case(TACHOMETER): {
		if(config->hasIncrementalEncoder()) {
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
		if(!config->hasLimitSwitchStart()) {
			nextStep();
		}
	}
	break;
	case(LIMIT_SWITCH_PIN_START): {
		step=LIMIT_SWITCH_PIN_END;
		if(!config->hasLimitSwitchEnd()) {
			nextStep();
		}
	}
	break;
	case(LIMIT_SWITCH_PIN_END): {
		step=LIMIT_SWITCH_DEBOUNCE;
		if(! (config->hasLimitSwitchStart() || config->hasLimitSwitchEnd())  ) {
			nextStep();
		}
	}
	break;
	case(LIMIT_SWITCH_DEBOUNCE): {
		if(STEP_MOTOR==config->type) {
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
		if(RC_SERVO==config->type) {
			step=RC_SERVO_PIN;
		} else {
			step=RANGE_START;
		}
	}
	break;
	case(RC_SERVO_PIN): {
		step=RANGE_START;
	}
	break;
	case(RANGE_START): {
		step=RANGE_SPAN;
	}
	break;
	case(RANGE_SPAN): {
		step=END_OF_OPERATION;
	}
	break;
	case(END_OF_OPERATION): {
		// Do nothing. Maybe this is an indication of error, and should be reported?
	}
	break;
	}
	byteIndex=0;
}
