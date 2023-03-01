#ifndef TESTCOMMSCHANNEL_HPP
#define TESTCOMMSCHANNEL_HPP

#include "comms/NetworkAddress.hpp"
#include "node/NodeRole.hpp"
#include "node/NodeType.hpp"
#include "security/Key.hpp"

#include <QHostAddress>
#include <QSignalSpy>
#include <QFileInfo>

class Associate;

class TestCommsChannel:public QObject
{
	Q_OBJECT

public:
	void testMultiple();
	void testCommsMock();

private slots:
	void testSingle();

};


QSharedPointer<Associate> generatePart(QString name, QSharedPointer<Key> key, NetworkAddress addr, NodeRole role, NodeType type );

#endif
// TESTCOMMSCHANNEL_HPP
