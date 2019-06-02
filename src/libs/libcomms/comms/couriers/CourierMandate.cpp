#include "CourierMandate.hpp"

#include "utility/string/String.hpp"

#include "uptime/MethodGate.hpp"




CourierMandate::CourierMandate(
	const quint16 payloadSize
	, const quint8 priority
	, const quint64 nextSend
	, const bool receiveActive
	, const bool sendActive
)
	:  payloadSize(payloadSize)
	, priority(priority)
	, nextSend(nextSend)
	, receiveActive(receiveActive)
	, sendActive(sendActive)
{
	OC_METHODGATE();
}



QString CourierMandate::toString()const
{
	OC_METHODGATE();
	return QString("CourierMandate(size=")+utility::string::humanReadableSize(payloadSize)+", pri="+QString::number(priority)+", nxt="+utility::string::humanReadableElapsedMS(nextSend)+", rx="+(receiveActive?"YES":"NO") +", tx="+(sendActive?"YES":"NO")+" )";
}


