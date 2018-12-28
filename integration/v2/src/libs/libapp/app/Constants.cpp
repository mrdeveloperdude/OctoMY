#include "app/Constants.hpp"

const qint64  Constants::MAX_SYNC_INTERVAL =		(1000 * 10); //Never sync more often than once every 10 sec
const QString Constants::ORGANIZATION_NAME =		"OctoMY™";
const QString Constants::DOMAIN_NAME =			"octomy.org™";
const QString Constants::BRAND_NAME =			"OctoMY™";
const QString Constants::APPLICATION_NAME_BASE =	BRAND_NAME;
const QString Constants::APPLICATION_VERSION =	"1.0";
const QString Constants::USERAGENT =				"OctoMY/"+APPLICATION_VERSION;




const QByteArray Constants::OCTOMY_USER_AGENT=				"OctoMY™ Zoo Client";
const QByteArray Constants::OCTOMY_SERVER=					"Apache/2.4.7 (Ubuntu)";
const QByteArray Constants::OCTOMY_API_VERSION_HEADER=		"x-octomy-api-v";
const QByteArray Constants::OCTOMY_API_VERSION_POINT_ONE=	"0.1";
const QByteArray Constants::OCTOMY_API_VERSION_CURRENT=		OCTOMY_API_VERSION_POINT_ONE;


const quint16 Constants::OCTOMY_UDP_BASE_PORT=			8123;
const quint16 Constants::OCTOMY_UDP_DEFAULT_PORT_AGENT=	OCTOMY_UDP_BASE_PORT+1;
const quint16 Constants::OCTOMY_UDP_DEFAULT_PORT_REMOTE=	OCTOMY_UDP_BASE_PORT+2;
const quint16 Constants::OCTOMY_UDP_DEFAULT_PORT_HUB=	OCTOMY_UDP_BASE_PORT+3;
const quint16 Constants::OCTOMY_UDP_DEFAULT_PORT_ZOO=	OCTOMY_UDP_BASE_PORT+0;


const QByteArray Constants::OCTOMY_ZOO_API_DO_DISCOVERY_ESCROW="doDiscoveryEscrow";









const QString Constants::AGENT_FACE_SPLITTER_MIDDLE_STATE="agent.face.splitter.middle.state";
const QString Constants::AGENT_FACE_SPLITTER_TOP_STATE="agent.face.splitter.top.state";
const QString Constants::AGENT_FACE_SPLITTER_BOTTOM_STATE="agent.face.splitter.bottom.state";


const QString Constants::AGENT_FACE_EYES_SHOW="agent.face.eyes.show";
const QString Constants::AGENT_FACE_LOG_SHOW="agent.face.log.show";
const QString Constants::AGENT_FACE_STATS_SHOW="agent.face.stats.show";
const QString Constants::AGENT_FACE_ONLINE_BUTTON_SHOW="agent.face.online_button.show";

const QString Constants::AGENT_CONNECT_BUTTON_COLOR="green";
const QString Constants::AGENT_DISCONNECT_COLOR="#1a3a52";


const QString Constants::AGENT_CONNECTION_STATUS="agent.connection.status";
