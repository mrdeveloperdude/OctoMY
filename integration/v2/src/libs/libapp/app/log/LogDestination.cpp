#include "LogDestination.hpp"

#include "uptime/MethodGate.hpp"

LogDestination::LogDestination()
{
	OC_METHODGATE();
}

LogDestination::~LogDestination()
{
	OC_METHODGATE();
}
