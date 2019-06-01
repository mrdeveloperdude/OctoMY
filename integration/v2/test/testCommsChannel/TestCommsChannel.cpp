#include "TestCommsChannel.hpp"

#include "../common_test/Common_test.hpp"
#include <QString>



OC_TEST_MAIN(test, TestCommsChannel)


QSharedPointer<Associate> generatePart(QString name, QSharedPointer<Key> key, NetworkAddress addr, NodeRole role, NodeType type )
{
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

// NOTE: The rest of the test code is distributed in other files.
