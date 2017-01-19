#ifndef ACTUATORSET_HPP
#define ACTUATORSET_HPP

#include "Actuator.hpp"
#include "DynamicArray.hpp"

#include "ActuatorFlags.hpp"

#include "ArduMY.hpp"


class ActuatorSet: public DynamicArray<Actuator>
{
public:

	uint32_t flags;

	// NOTE: There is currently no inhrent mechanism to set the dirty flags. It must be carried out by the same process that makes the respective changes

	ACTUATOR_FLAG_SELECTOR(isSyncDue,					setSyncDue,					0 )
	ACTUATOR_FLAG_SELECTOR(isCountDirty,				setCountDirty,				1 )
	ACTUATOR_FLAG_SELECTOR(isConfigDirty,				setConfigDirty,				2 )
	ACTUATOR_FLAG_SELECTOR(isValuesDirty,				setValuesDirty,				3 )

	//NOTE: It is expected that when the number of actuators decrease, the remaining actuators will retain their previous configuration.
	//NOTE: It is expected that when the number of actuators increase, the new actuators will be reset to default configuration.

public:
	ActuatorSet(uint32_t initialSize=0)
		: DynamicArray<Actuator>(initialSize)
		, flags(0x00)
	{
		//cout<< "ActuatorSet CTOR\n";
	}

	virtual ~ActuatorSet()
	{

	}


	bool isEqual(const ActuatorSet &other) const
	{
		if(size()!=other.size()) {
			return false;
		}

		const uint32_t sz=size();
		for(uint32_t i=0; i<sz; ++i) {
			const Actuator &a=operator [](i);
			const Actuator &b=other[i];
			if(!a.isEqual(b)) {
				return false;
			}
		}
		// By not failing until now we win!
		return true;
	}

};

bool operator== (const ActuatorSet &a, const ActuatorSet &b);
bool operator!= (const ActuatorSet &a, const ActuatorSet &b);


#endif // ACTUATORSET_HPP
