#ifndef ACTUATORSET_HPP
#define ACTUATORSET_HPP

#include "Actuator.hpp"
#include "DynamicArray.hpp"


#include "ArduMY.hpp"

class ActuatorSet: public DynamicArray<Actuator>
{
public:
	ActuatorSet(unsigned int initialSize=0)
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

		const unsigned int sz=size();
		for(unsigned int i=0; i<sz; ++i) {
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
