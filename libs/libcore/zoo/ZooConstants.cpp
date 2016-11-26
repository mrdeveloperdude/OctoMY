#include "ZooConstants.hpp"

const QByteArray ZooConstants::OCTOMY_USER_AGENT=				"OctoMY™ Zoo Client";
const QByteArray ZooConstants::OCTOMY_SERVER=					"Apache/2.4.7 (Ubuntu)";
const QByteArray ZooConstants::OCTOMY_API_VERSION_HEADER=		"x-octomy-api-v";
const QByteArray ZooConstants::OCTOMY_API_VERSION_POINT_ONE=	"0.1";
const QByteArray ZooConstants::OCTOMY_API_VERSION_CURRENT=		OCTOMY_API_VERSION_POINT_ONE;



const quint16 ZooConstants::OCTOMY_UDP_BASE_PORT=			8123;
const quint16 ZooConstants::OCTOMY_UDP_DEFAULT_PORT_AGENT=	OCTOMY_UDP_BASE_PORT+1;
const quint16 ZooConstants::OCTOMY_UDP_DEFAULT_PORT_REMOTE=	OCTOMY_UDP_BASE_PORT+2;
const quint16 ZooConstants::OCTOMY_UDP_DEFAULT_PORT_HUB=	OCTOMY_UDP_BASE_PORT+3;
const quint16 ZooConstants::OCTOMY_UDP_DEFAULT_PORT_ZOO=	OCTOMY_UDP_BASE_PORT+0;

////////////////////////////////////////////////////////////////////////////////



const QByteArray ZooConstants::OCTOMY_ZOO_API_DO_DISCOVERY_ESCROW="doDiscoveryEscrow";


ZooConstants::ZooConstants()
{

}
