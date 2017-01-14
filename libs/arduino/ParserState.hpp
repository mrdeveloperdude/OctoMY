#ifndef PARSERSTATE_HPP
#define PARSERSTATE_HPP

enum ParserState {
	OCTOMY_SYNC=0 //We decided on our own to sync
	, OCTOMY_AWAITING_COMMAND // We were told to sync
	, OCTOMY_SET_ACTUATOR_COUNT // The number of servos has changed
	, OCTOMY_SET_ACTUATOR_CONFIG // The configuration of one or more servos has changed
	, OCTOMY_SET_ACTUATOR_VALUES // The positions of one or more servos has changed
	, OCTOMY_SET_ACTUATOR_LIMP // The limp status of servos has changed
	, OCTOMY_UNKNOWN // Unknown command. Will translate to implicity sync
	, OCTOMY_COUNT
};


#endif // PARSERSTATE_HPP
