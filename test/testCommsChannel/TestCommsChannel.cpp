#include "TestCommsChannel.hpp"

#include <QTest>
#include <QString>

QTEST_MAIN(TestCommsChannel)


void heading(QString msg, QString ch){
	auto sz=msg.size();
	auto full=80-4;
	auto half=(full-sz)/2;
	auto second=full-sz-half;
	QTest::waitForEvents();
	qDebug()<<"";
	qDebug()<<"";
	qDebug().nospace().noquote()<<QString(ch).repeated(half)<< " # "<<msg << " # "<<QString(ch).repeated(second);
	qDebug()<<"";
}




QSharedPointer<NodeAssociate> generatePart(QString name, QSharedPointer<Key> key, NetworkAddress addr, DiscoveryRole role, DiscoveryType type ){
	QVariantMap peerMap;
	peerMap["publicAddress"]=addr.toVariantMap();
	peerMap["localAddress"]=addr.toVariantMap();
	peerMap["lastSeenMS"]=0;
	peerMap["birthDate"]=0;
	peerMap["key"]=key->toVariantMap(true);
	peerMap["role"]=DiscoveryRoleToString(role);
	peerMap["type"]=DiscoveryTypeToString(type);
	peerMap["name"]=name;
	peerMap["gender"]="Male";
	peerMap["trusts"]=QStringList();
	QSharedPointer<NodeAssociate> part(new NodeAssociate(peerMap));
	return part;
}
