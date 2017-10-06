#ifndef DISCOVERYROLE_HPP
#define DISCOVERYROLE_HPP

#include <QDebug>


enum DiscoveryRole{
	ROLE_AGENT
	, ROLE_CONTROL
	, ROLE_UNKNOWN
};

const QDebug &operator<<(QDebug &d, const DiscoveryRole &role);

QString DiscoveryRoleToString(DiscoveryRole s);

DiscoveryRole DiscoveryRoleFromString(QString s);

#endif // DISCOVERYROLE_HPP
