#include "CourierMandate.hpp"

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
