#ifndef DISCOVERYROLE_HPP
#define DISCOVERYROLE_HPP

#include <QDebug>


enum DiscoveryRole{
	ROLE_AGENT
	, ROLE_CONTROL
	, ROLE_UNKNOWN
};



enum DiscoveryType{
	TYPE_AGENT
	, TYPE_REMOTE
	, TYPE_HUB
	, TYPE_ZOO
	, TYPE_UNKNOWN
};



const QDebug &operator<<(QDebug &d, const DiscoveryRole &role);
const QDebug &operator<<(QDebug &d, const DiscoveryType &type);


QString DiscoveryRoleToString(DiscoveryRole s);
QString DiscoveryTypeToString(DiscoveryType s);


DiscoveryRole DiscoveryRoleFromString(QString s);
DiscoveryType DiscoveryTypeFromString(QString s);

#endif // DISCOVERYROLE_HPP
