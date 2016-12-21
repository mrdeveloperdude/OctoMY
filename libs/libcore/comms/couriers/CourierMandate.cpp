#include "CourierMandate.hpp"

#include "../libutil/utility/Utility.hpp"

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

}



QString CourierMandate::toString()const
{
	return QString("CourierMandate(size=")+utility::humanReadableSize(payloadSize)+", pri="+QString::number(priority)+", nxt="+utility::humanReadableElapsedMS(nextSend)+", rx="+(receiveActive?"YES":"NO") +", tx="+(sendActive?"YES":"NO")+" )";
}


