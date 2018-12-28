#include "uptime/MethodGate.hpp"

#include <QDebug>

MethodGate::MethodGate(const void *parentThis)
{
	if((reinterpret_cast<const long long unsigned int *>(this)) < reinterpret_cast<const long long unsigned int *>(OC_LOWEST_POSSIBLE_ADDRESS)) {
		qWarning()<<"ERROR: this < "<<OC_LOWEST_POSSIBLE_ADDRESS<<", which likely means nullptr access";
	}
	if((reinterpret_cast<const long long unsigned int *>(parentThis)) < reinterpret_cast<const long long unsigned int *>(OC_LOWEST_POSSIBLE_ADDRESS)) {
		qWarning()<<"ERROR: parentThis < "<<OC_LOWEST_POSSIBLE_ADDRESS<<", which likely means nullptr access";
	}
}



FunctionGate::FunctionGate()
{

}
