#include "NodeType.hpp"


#include "app/Constants.hpp"

#include <QColor>


const QDebug &operator<<(QDebug &d, const NodeType nodeType)
{
	switch(nodeType) {
	case(TYPE_AGENT): {
		d.nospace() << "TYPE_AGENT";
	}
	break;
	case(TYPE_REMOTE): {
		d.nospace() << "TYPE_REMOTE";
	}
	break;
	case(TYPE_HUB): {
		d.nospace() << "TYPE_HUB";
	}
	break;
	case(TYPE_ZOO): {
		d.nospace() << "TYPE_ZOO";
	}
	break;
	default:
		d.nospace() << "TYPE_UNKNOWN";
	}
	return d.maybeSpace();
}


QString nodeTypeToString(NodeType nodeType)
{
#define  NodeTypeToStringCASE(A) case (A):return #A
	switch(nodeType) {
		NodeTypeToStringCASE(TYPE_AGENT);
		NodeTypeToStringCASE(TYPE_REMOTE);
		NodeTypeToStringCASE(TYPE_HUB);
		NodeTypeToStringCASE(TYPE_ZOO);
	//default:
//        return "UNKNOWN";
		NodeTypeToStringCASE(TYPE_UNKNOWN);
	}
#undef NodeTypeToStringCASE
	return "UNKNOWN";
}



NodeType nodeTypeFromString(QString string)
{
	if("TYPE_AGENT"==string) {
		return TYPE_AGENT;
	}
	if("TYPE_REMOTE"==string) {
		return TYPE_REMOTE;
	}
	if("TYPE_HUB"==string) {
		return TYPE_HUB;
	}
	if("TYPE_ZOO"==string) {
		return TYPE_ZOO;
	}
	return TYPE_UNKNOWN;
}



quint16 defaultPortForNodeType(NodeType nodeType)
{
	quint16 defaultPort=0;
	switch(nodeType) {
	case(TYPE_ZOO):
	//default:
	case(TYPE_UNKNOWN):
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
