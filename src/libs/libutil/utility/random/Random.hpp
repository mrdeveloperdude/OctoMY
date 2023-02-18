#ifndef UTILITY_RANDOM_HPP
#define UTILITY_RANDOM_HPP

#include <QByteArray>

namespace utility
{



namespace random
{

void qsrand(quint32 s);
quint32 qrand();
float frand();
double dfrand();

QByteArray randomByteArray(int size);


}

}


#define randomBit() (utility::random::frand()<0.5f)


#endif
// UTILITY_RANDOM_HPP

