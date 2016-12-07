#include "CourierMandate.hpp"

#include "../libutil/utility/Utility.hpp"

CourierMandate::CourierMandate(
	const quint16 payloadSize
	, const quint8 priority
	, const quint64 interval
	, const bool receiveActive
	, const bool sendActive
)
	:  payloadSize(payloadSize)
	, priority(priority)
	, interval(interval)
	, receiveActive(receiveActive)
	, sendActive(sendActive)
{

}



QString CourierMandate::toString()const
{
	return QString("CourierMandate(size=")+utility::humanReadableSize(payloadSize)+", pri="+QString::number(priority)+", ival="+utility::humanReadableElapsedMS(interval)+", rx="+(receiveActive?"YES":"NO") +", tx="+(sendActive?"YES":"NO")+" )";
}


