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

	ACTUATOR_FLAG_SELECTOR(isSyncDue,					setSyncDue,					0 )
	ACTUATOR_FLAG_SELECTOR(isCountDirty,				setCountDirty,				1 )
	ACTUATOR_FLAG_SELECTOR(isConfigDirty,				setConfigDirty,				2 )
	ACTUATOR_FLAG_SELECTOR(isValuesDirty,				setValuesDirty,				3 )

public:
	ActuatorSet(uint32_t initialSize=0)
		: DynamicArray<Actuator>(initialSize)
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
