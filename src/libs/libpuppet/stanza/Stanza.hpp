#ifndef STANZA_HPP
#define STANZA_HPP

#include "uptime/SharedPointerWrapper.hpp"

class Lobe;
class PoseMapping;

class Stanza
{
private:
	QString mID;
	QString mLobeName;
	QSharedPointer<Lobe> mLobe;
	QSharedPointer<PoseMapping> mMapping;
public:
	Stanza();
	explicit Stanza(const QVariantMap map);

public:
	QString id() const;
	void setID(const QString &id);
	QSharedPointer<Lobe> lobe();
	QSharedPointer<PoseMapping> mapping();
	
	void setLobe(QSharedPointer<Lobe>);
	void setMapping(QSharedPointer<PoseMapping>);
	
	
public: // Converting selectors to/from different formats
	QVariantMap toVariantMap();
	void fromVariantMap(const QVariantMap map);
	//QString toString();
	//QMap<QString, QString> toMap();

};

#endif // STANZA_HPP
