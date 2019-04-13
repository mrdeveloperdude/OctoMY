#ifndef NODETYPE_HPP
#define NODETYPE_HPP

#include <QDebug>

enum NodeType: quint8 {
	TYPE_AGENT
	, TYPE_REMOTE
	, TYPE_HUB
	, TYPE_ZOO
	, TYPE_UNKNOWN
};

const QDebug &operator<<(QDebug &d, const NodeType nodeType);

QString nodeTypeToString(NodeType nodeType);

NodeType nodeTypeFromString(QString string);


quint16 defaultPortForNodeType(NodeType nodeType);

QColor defaultColorForNodeType(NodeType nodeType);

#endif // NODETYPE_HPP
