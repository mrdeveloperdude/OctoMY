#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <QString>
#include <QColor>


class Constants
{
public:


	// TODO: Integrate constants from settings


	static const qint64 MAX_SYNC_INTERVAL;

	static const QString ORGANIZATION_NAME;
	static const QString DOMAIN_NAME;
	static const QString BRAND_NAME;
	static const QString APPLICATION_NAME_BASE;
	static const QString APPLICATION_VERSION;
	static const QString USERAGENT;



	static const qint64 OCTOMY_WEB_TIMEOUT;


	static const QByteArray OCTOMY_USER_AGENT;

	static const QByteArray OCTOMY_SERVER;

	static const QByteArray OCTOMY_API_VERSION_HEADER;

	static const QByteArray OCTOMY_API_VERSION_POINT_ONE;
	static const QByteArray OCTOMY_API_VERSION_CURRENT;

	static const QByteArray OCTOMY_ZOO_API_DO_DISCOVERY_ESCROW;


	static const QString ACCEPT_HEADER;
	static const QString APPLICATION_JSON_MIME;




	static const quint16 OCTOMY_UDP_BASE_PORT;
	static const quint16 OCTOMY_UDP_DEFAULT_PORT_AGENT;
	static const quint16 OCTOMY_UDP_DEFAULT_PORT_REMOTE;
	static const quint16 OCTOMY_UDP_DEFAULT_PORT_HUB;
	static const quint16 OCTOMY_UDP_DEFAULT_PORT_ZOO;
	static const quint16 OCTOMY_UDP_DEFAULT_PORT_UNKNOWN;


	static const qint32 OCTOMY_WEB_COLLECT_AT_MOST;

	static const QString AGENT_FACE_SPLITTER;

	static const QString AGENT_FACE_EYES_SHOW;
	static const QString AGENT_FACE_LOG_SHOW;
	static const QString AGENT_FACE_STATS_SHOW;
	static const QString AGENT_FACE_ONLINE_BUTTON_SHOW;

	static const QString AGENT_CONNECT_BUTTON_COLOR;
	static const QString AGENT_DISCONNECT_COLOR;

	static const QString AGENT_CONNECTION_STATUS;


	static const bool OC_LOG_CONFIGURE_HELPER_WARNINGS;
	static const bool OC_LOG_CONFIGURE_HELPER_CHANGES;


	static const int OC_STATUS_BLINK_TIME;


	static const QString dateFMTYear;
	static const QString dateFMTMonth;
	static const QString dateFMTDay;
	static const QString dateFMTHour;
	static const QString dateFMTMinute;
	static const QString dateFMTSecond;
	static const QString dateFMTMillisecond;
	static const QString dateFMTPortable;

	static const QString OC_NODE_TYPE_AGENT_NAME;
	static const QString OC_NODE_TYPE_REMOTE_NAME;
	static const QString OC_NODE_TYPE_HUB_NAME;
	static const QString OC_NODE_TYPE_ZOO_NAME;
	static const QString OC_NODE_TYPE_UNKNOWN_NAME;

	static const QString OC_NODE_ROLE_AGENT_NAME;
	static const QString OC_NODE_ROLE_CONTROL_NAME;
	static const QString OC_NODE_ROLE_UNKNOWN_NAME;

	static const QColor OC_NODE_TYPE_AGENT_COLOR;
	static const QColor OC_NODE_TYPE_REMOTE_COLOR;
	static const QColor OC_NODE_TYPE_HUB_COLOR;
	static const QColor OC_NODE_TYPE_ZOO_COLOR;
	static const QColor OC_NODE_TYPE_UNKNOWN_COLOR;

};

#endif // CONSTANTS_HPP

