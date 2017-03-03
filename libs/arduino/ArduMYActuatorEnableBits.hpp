#ifndef ARDUMYACTUATORENABLEBITS_HPP
#define ARDUMYACTUATORENABLEBITS_HPP

/**
 * @brief The ArduMYActuatorEnableBits class is meant to separate the enable-bits
 * code into its own testable class.
 *
 * TODO: Perform the actual separation.
 *
 */

class ArduMYActuatorEnableBits
{
public:
	ArduMYActuatorEnableBits();

	void serialize();
	void parse();
};

#endif // ARDUMYACTUATORENABLEBITS_HPP
