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




QSharedPointer<Associate> generatePart(QString name, QSharedPointer<Key> key, NetworkAddress addr, NodeRole role, NodeType type ){
	QVariantMap peerMap;
	peerMap["publicAddress"]=addr.toVariantMap();
	peerMap["localAddress"]=addr.toVariantMap();
	peerMap["lastSeenMS"]=0;
	peerMap["birthDate"]=0;
	peerMap["key"]=key->toVariantMap(true);
	peerMap["role"]=nodeRoleToString(role);
	peerMap["type"]=nodeTypeToString(type);
	peerMap["name"]=name;
	peerMap["gender"]="Male";
	peerMap["trusts"]=QStringList();
	QSharedPointer<Associate> part(OC_NEW Associate(peerMap));
	return part;
}
