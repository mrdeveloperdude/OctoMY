#ifndef NODEROLE_HPP
#define NODEROLE_HPP

#include <QDebug>


enum NodeRole: quint8 {
	ROLE_AGENT
	, ROLE_CONTROL
	, ROLE_UNKNOWN
};

const QDebug &operator<<(QDebug &d, const NodeRole &role);

QString nodeRoleToString(NodeRole s);

NodeRole nodeRoleFromString(QString s);

#endif
// NODEROLE_HPP
