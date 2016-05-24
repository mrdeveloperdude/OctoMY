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


		static const QByteArray OCTOMY_ZOO_API_GET_NODE_CRUMB;
		static const QByteArray OCTOMY_ZOO_API_PUT_NODE_CRUMB;

		static const QByteArray OCTOMY_ZOO_API_UDP_PUNCH;

	public:
		ZooConstants();
};

#endif // ZOOCONSTANTS_HPP
