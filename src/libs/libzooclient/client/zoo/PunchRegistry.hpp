#ifndef PUNCHREGISTRY_HPP
#define PUNCHREGISTRY_HPP

#include <QDateTime>
#include <QMap>
#include <QVariantMap>


struct PunchPartner {

	QString public_address;
	quint16 public_port;
	QString local_address;
	quint16 local_port;
	quint64 lastTime;


	PunchPartner();
	PunchPartner(QString public_address, quint16 public_port, QString local_address, quint16 local_port);
	//PunchPartner(const PunchPartner &o);
	bool operator==(const PunchPartner &o) const;
	bool isSet();
	QVariantMap toVariantMap();

};



struct UDPPunch {

	PunchPartner a,b;
	QString token;

	UDPPunch(QString token);

	bool set(PunchPartner p);

	PunchPartner theOther(PunchPartner p);
	QVariantMap toVariantMap();

};



class PunchRegistry: public QMap<QString, UDPPunch *>
{
public:
	bool update(QString punchToken, QString public_address, quint16 public_port, QString local_address, quint16 local_port);


};

#endif
// PUNCHREGISTRY_HPP
