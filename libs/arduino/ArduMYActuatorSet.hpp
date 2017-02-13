#ifndef ACTUATORSET_HPP
#define ACTUATORSET_HPP

#include "ArduMY.hpp"


#include "ArduMYActuator.hpp"
#include "DynamicArray.hpp"

#include "ArduMYActuatorFlags.hpp"


class ArduMYActuatorSet: public DynamicArray<ArduMYActuator>
{
public:

	uint32_t flags;

	// NOTE: There is currently no inhrent mechanism to set the dirty flags. It must be carried out by the same process that makes the respective changes

	ARDUMY_ACTUATOR_FLAG_SELECTOR(isSyncDue,					setSyncDue,					0 )
	ARDUMY_ACTUATOR_FLAG_SELECTOR(isCountDirty,					setCountDirty,				1 )
	ARDUMY_ACTUATOR_FLAG_SELECTOR(isConfigDirty,				setConfigDirty,				2 )
	ARDUMY_ACTUATOR_FLAG_SELECTOR(isValuesDirty,				setValuesDirty,				3 )

	//NOTE: It is expected that when the number of actuators decrease, the remaining actuators will retain their previous configuration.
	//NOTE: It is expected that when the number of actuators increase, the new actuators will be reset to default configuration.

public:
	ArduMYActuatorSet(uint32_t initialSize=0)
		: DynamicArray<ArduMYActuator>(initialSize)
		, flags(0x00)
	{
		//cout<< "ActuatorSet CTOR\n";
	}

	virtual ~ArduMYActuatorSet()
	{

	}


	bool isEqual(const ArduMYActuatorSet &other) const
	{
		if(size()!=other.size()) {
			return false;
		}

		const uint32_t sz=size();
		for(uint32_t i=0; i<sz; ++i) {
			const ArduMYActuator &a=operator [](i);
			const ArduMYActuator &b=other[i];
			if(!a.isEqual(b)) {
				return false;
			}
		}
		// By not failing until now we win!
		return true;
	}

};

bool operator== (const ArduMYActuatorSet &a, const ArduMYActuatorSet &b);
bool operator!= (const ArduMYActuatorSet &a, const ArduMYActuatorSet &b);


#endif // ACTUATORSET_HPP
