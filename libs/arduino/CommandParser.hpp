#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP

#include "DynamicArray.hpp"
#include "Actuator.hpp"
#include "ParserState.hpp"
#include "MagicDetector.hpp"
#include "ActuatorValueParser.hpp"
#include "ActuatorConfigParser.hpp"

/*

  Protocol:

  Sync means a pre-shared 4-byte magic sequence is sent, allowing recepient to reset parsing to a well defined point in the stream.
  TODO: Incorporate some form of escaping to ensure that this magic sequence does not ever arrise by accident

  Frame means all the data pertaining to one single command. The frame starts with 1 byte indicating the command type.

  The rest of the frame are the "arguments" or data associated with the command, and the number of bytes required may vary.

  Initial comms after hardware reset is sync. After this, sync is introduced at interval to allow recepient to re-gain communication after falling out of sync.


  OCTOMY_SET_SERVO_COUNT -	The number of servos is by default 2. The protocol can extend this number, and this is usually done at initialization before the servos are used
							This command is accompanied by 1 byte representing the number of servos

  OCTOMY_SET_SERVO_POSITION - This command is accompanied by (servocount+7)/8 bytes, where each bit represents one servo. For all enabled bits, a 4 byte float will follow containing the absolute virtual position of that servo

  OCTOMY_SET_SERVO_LIMP - This command is accompanied by (servocount+7)/8 bytes, where each bit represents the limp status for that servo, where true = limp.

  OCTOMY_SET_SERVO_CONFIG - This command is accompanied by (servocount+7)/8 bytes, where each bit represents one servo. For all enabled bits, a set of configuration values will follow for that servo
							The format of the config data is as follows: flags:enabled;continuous_rotation;auto_limp, range_low, range_high, center_position, hardware_port,

*/

struct CommandParser {

	DynamicArray<Actuator> actuators;
	ParserState currentCommand;
	const uint8_t magic[4];
	MagicDetector magicDetector;
	ActuatorConfigParser servoConfigParser;
	ActuatorValueParser servoPosParser;
	bool dirtyServos;

	explicit CommandParser();

	// If bad command is detected, go into "wait for sync" before we try parsing a command again,
	// and if not, reset states for parsing the command at hand
	ParserState prepareCommand(const int8_t inChar);

	void parse(const int8_t inChar);
};

#endif // COMMANDPARSER_HPP
