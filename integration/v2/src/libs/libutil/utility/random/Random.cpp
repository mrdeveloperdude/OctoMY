#include "utility/random/Random.hpp"

#include <QtMath>

namespace utility
{

namespace random
{


float frand()
{
	float v=qrand();
	v/=RAND_MAX;
	return v;
}

double dfrand()
{
	double v=qrand();
	v/=RAND_MAX;
	return v;
}

}

}
