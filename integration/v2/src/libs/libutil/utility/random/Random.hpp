#ifndef UTILITY_RANDOM_HPP
#define UTILITY_RANDOM_HPP

namespace utility
{



namespace random
{

float frand();
double dfrand();

}

}


#define randomBit() (utility::random::frand()<0.5f)


#endif
// UTILITY_RANDOM_HPP

