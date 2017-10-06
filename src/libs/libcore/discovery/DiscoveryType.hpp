#ifndef DISCOVERYTYPE_HPP
#define DISCOVERYTYPE_HPP

#include <QDebug>

enum DiscoveryType{
	TYPE_AGENT
	, TYPE_REMOTE
	, TYPE_HUB
	, TYPE_ZOO
	, TYPE_UNKNOWN
};

const QDebug &operator<<(QDebug &d, const DiscoveryType &type);

QString DiscoveryTypeToString(DiscoveryType s);

DiscoveryType DiscoveryTypeFromString(QString s);

#endif // DISCOVERYTYPE_HPP
