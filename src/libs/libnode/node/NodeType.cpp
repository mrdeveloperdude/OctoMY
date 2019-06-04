#include "NodeType.hpp"

#include "app/Constants.hpp"

#include <QColor>


const QDebug &operator<<(QDebug &d, const NodeType nodeType)
{
	switch(nodeType) {
	case(TYPE_AGENT): {
		d.nospace() << Constants::OC_NODE_TYPE_AGENT_NAME;
	}
	break;
	case(TYPE_REMOTE): {
		d.nospace() << Constants::OC_NODE_TYPE_REMOTE_NAME;
	}
	break;
	case(TYPE_HUB): {
		d.nospace() << Constants::OC_NODE_TYPE_HUB_NAME;
	}
	break;
	case(TYPE_ZOO): {
		d.nospace() << Constants::OC_NODE_TYPE_ZOO_NAME;
	}
	break;
	default:
		d.nospace() << Constants::OC_NODE_TYPE_UNKNOWN_NAME;
	}
	return d.maybeSpace();
}


QString nodeTypeToString(NodeType nodeType)
{
	switch(nodeType) {
	case (TYPE_AGENT):
		return Constants::OC_NODE_TYPE_AGENT_NAME;
	case (TYPE_REMOTE):
		return Constants::OC_NODE_TYPE_REMOTE_NAME;
	case (TYPE_HUB):
		return Constants::OC_NODE_TYPE_HUB_NAME;
	case (TYPE_ZOO):
		return Constants::OC_NODE_TYPE_ZOO_NAME;
	default:
		return Constants::OC_NODE_TYPE_UNKNOWN_NAME;
	}
}


NodeType nodeTypeFromString(QString string)
{
	if(Constants::OC_NODE_TYPE_AGENT_NAME==string) {
		return TYPE_AGENT;
	}
	if(Constants::OC_NODE_TYPE_REMOTE_NAME==string) {
		return TYPE_REMOTE;
	}
	if(Constants::OC_NODE_TYPE_HUB_NAME==string) {
		return TYPE_HUB;
	}
	if(Constants::OC_NODE_TYPE_ZOO_NAME==string) {
		return TYPE_ZOO;
	}
	return TYPE_UNKNOWN;
}


quint16 defaultPortForNodeType(NodeType nodeType)
{
	quint16 defaultPort=0;
	switch(nodeType) {
	case(TYPE_ZOO): {
		defaultPort=Constants::OCTOMY_UDP_DEFAULT_PORT_ZOO;
	}
	break;
	case(TYPE_AGENT): {
		defaultPort=Constants::OCTOMY_UDP_DEFAULT_PORT_AGENT;
	}
	break;
	case(TYPE_REMOTE): {
		defaultPort=Constants::OCTOMY_UDP_DEFAULT_PORT_REMOTE;
	}
	break;
	case(TYPE_HUB): {
		defaultPort=Constants::OCTOMY_UDP_DEFAULT_PORT_HUB;
	}
	break;
	//case(TYPE_UNKNOWN):
	default: {
		defaultPort=Constants::OCTOMY_UDP_DEFAULT_PORT_UNKNOWN;
	}
	break;
	}
	return defaultPort;
}


QColor defaultColorForNodeType(NodeType nodeType)
{
	switch(nodeType) {
	case(TYPE_AGENT):
		return QColor ("#e83636");
	case(TYPE_REMOTE):
		return QColor ("#36bee8");
	case(TYPE_HUB):
		return QColor ("#36e843");
	case(TYPE_ZOO):
		return QColor ("orange");
	// Signal error
	default:
		return QColor ("yellow");
	}
	//return Qt::red;
}
