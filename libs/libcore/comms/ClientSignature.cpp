#include "ClientSignature.hpp"


ClientSignature::ClientSignature(const quint32 platform, const quint32 executable, const QHostAddress host, const quint16 port)
	: platform(platform)
	, executable(executable)
	, host(host)
	, port(port)
{
}


const QString ClientSignature::toString() const
{
	return ""+QString::number(platform,16)+":"+QString::number(executable,16)+"-"+host.toString()+":"+QString::number(port);
}
