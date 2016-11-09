#ifndef DISCOVERYCLIENT_HPP
#define DISCOVERYCLIENT_HPP


#include "DiscoveryRole.hpp"
#include "NodeAssociateStore.hpp"
#include "basic/NodeAssociate.hpp"


#include <QTimer>
#include <QObject>
#include <QDateTime>
#include <QUrl>




class Node;

namespace qhttp
{
namespace client
{
class QHttpClient;
}
}



/**
  The discovery and pairing for OctoMY™ nodes are as follows:

	1. Establish physical proximity (in order of preference from security standpoint):
		a. NFC: by having NFC range physical proximity is implied
		b. Cam/QR: by having scanning QR with camera, physical proximity is implied
		c. Bluetooth: by having bluetooth range physical proximity is implied
		d. Zoo: posting exiring gps coords together with pairing signature
		e. LAN: Identify common gateway
	2. Exchange pairing signatures and pubkeys
	3. Exchange pairing challenge/responses to verify legitimity of signature/pubkeys
	4. Show identicon for signatures in list on nodes for user to verify
	5. Rack up multifactors and update list to show the "security level"

	As soon as a node is verified all the way into the list of another
	(aka "discovered"), it's then up to the user to verify that the signatures
	match by inspecting the	respective identicons closely.

	Discovery is done separately from pairing. This means that signatures that
	has been discovered previously will remain discovered regardless if the user
	has selected to trust them or not. In fact the user is free to
	trust/dis-trust/re-trust any of the discovered nodes as many times
	as she likes.

	Removing discovery records is considered an advanced operation that users
	will not need to conduct unless they are in fact debugging the pairing protocol.


	Glossary:

	+ Discovery: A comlpetely automatic process whereby nodes find eachother and
	  exchange the necessary details to be able to unmistakably verify the
	  identity of eachother in the future. May be aided by manual process like
	  camera discovery. NOTE: discovery is the hard part, pairing is just UI.

	+ Pairing: A manual process whereby the operator assigns to a previously
	  dicovered node any amount of trust.

	+ Node: Any participant in an OctoMY network, such as agent, hub or remote.

	+ Client: All data pertaining to another Node that this node knows about.

	+ Birth/Delivery: The transition of a node from uninitialized to initialized.
	  Before the node is initialized, it does not have the necessary
	  identification and security primitives to parttake in discovery and pairing.


*/


class DiscoveryClient: public QObject
{
	Q_OBJECT
private:
	QTimer mTimer;
	quint64 mLastZooPair;
	QUrl  mServerURL;
	qhttp::client::QHttpClient     *mClient;
	Node &mNode;
	Key &mKey;

private:
	void registerPossibleParticipant(QVariantMap map);

public:
	DiscoveryClient(Node &node);

public:

	void setURL(const QUrl&);
	void start();
	void stop();

	void discover();

	Node &getNode();

private slots:

	void onTimer();

signals:

	void nodeDiscovered(QString id);
};

#endif // DISCOVERYCLIENT_HPP
