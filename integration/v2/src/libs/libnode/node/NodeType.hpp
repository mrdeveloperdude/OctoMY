#ifndef NODETYPE_HPP
#define NODETYPE_HPP

#include <QDebug>

enum NodeType: quint8{
	TYPE_AGENT
	, TYPE_REMOTE
	, TYPE_HUB
	, TYPE_ZOO
	, TYPE_UNKNOWN
};

const QDebug &operator<<(QDebug &d, const NodeType &type);

QString nodeTypeToString(NodeType s);

NodeType nodeTypeFromString(QString s);

#endif // NODETYPE_HPP
