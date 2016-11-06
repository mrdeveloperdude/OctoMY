#ifndef ZOOCONSTANTS_HPP
#define ZOOCONSTANTS_HPP

#include <QByteArray>

class ZooConstants
{
public:
	static const QByteArray OCTOMY_USER_AGENT;

	static const QByteArray OCTOMY_SERVER;

	static const QByteArray OCTOMY_API_VERSION_HEADER;

	static const QByteArray OCTOMY_API_VERSION_POINT_ONE;
	static const QByteArray OCTOMY_API_VERSION_CURRENT;

	static const QByteArray OCTOMY_ZOO_API_DO_DISCOVERY_ESCROW;

	static const quint16 OCTOMY_UDP_BASE_PORT;
	static const quint16 OCTOMY_UDP_DEFAULT_PORT_AGENT;
	static const quint16 OCTOMY_UDP_DEFAULT_PORT_REMOTE;
	static const quint16 OCTOMY_UDP_DEFAULT_PORT_HUB;
	static const quint16 OCTOMY_UDP_DEFAULT_PORT_ZOO;

public:
	ZooConstants();
};

#endif // ZOOCONSTANTS_HPP
