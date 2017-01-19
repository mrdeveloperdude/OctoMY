#ifndef COMMANDSERIALIZER_HPP
#define COMMANDSERIALIZER_HPP

#include "ParserState.hpp"
#include "MagicDetector.hpp"
#include "ActuatorValueSerializer.hpp"
#include "ActuatorConfigSerializer.hpp"
#include "ActuatorSet.hpp"

class CommandSerializer
{
public:
	ActuatorSet &actuators;
	ParserState currentCommand;
	const uint8_t magic[4];
	uint8_t byteIndex;
	ActuatorConfigSerializer actuatorConfigSerializer;
	int16_t actuatorConfigIndex;
	ActuatorValueSerializer actuatorValuesSerializer;

public:
	CommandSerializer();


	bool isReadyForNewCommand();
	void startSync();
	void startActuatorCount();
	void startActuatorConfig(uint8_t index);
	void startActuatorValues();

public:

	// Return true if there is more data remaining to be read
	bool hasMoreData() const;

	// Get the next byte of serial data. NOTE: only valid when hasMoreData() returns true, if not will return 0x00
	uint8_t nextByte();
};

#endif // COMMANDSERIALIZER_HPP
