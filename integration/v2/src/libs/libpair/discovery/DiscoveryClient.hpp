#ifndef DISCOVERYCLIENT_HPP
#define DISCOVERYCLIENT_HPP

#include "node/NodeRole.hpp"
#include "AddressBook.hpp"
#include "address/Associate.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "utility/time/HoneymoonScheduler.hpp"

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
  The discovery and pairing for OctoMY™ nodes works as follows:

	Discovery is the mechanism in which nodes seek out and establish secure
	connections with eachother. Pairing is the mechanism in which the user will
	assign a level of trust to already discovered nodes.

	Discovery is done separately from pairing. This means that signatures that
	has been discovered previously will remain discovered regardless if the user
	has selected to trust them or not. In fact the user is free to
	trust/dis-trust/re-trust any of the discovered nodes as many times
	as she likes.

	Removing discovery records is considered an advanced operation that users
	will not need to conduct unless they are in fact debugging the pairing protocol.

	Discovery proceeds as follows:

	1. Physical proximity is established (in order of preference from security standpoint):
		a. NFC: by having NFC range physical proximity is implied
		b. Cam/QR: by having scanning QR with camera, physical proximity is implied
		c. Bluetooth: by having bluetooth range physical proximity is implied
		d. Zoo: posting expiring gps coordinates together with pairing signature
		e. LAN: Identify common gateway
	2. Pairing signatures and pubkeys are exchanged
	3. Pairing challenge/responses are exchanged to verify legitimity of signature/pubkeys
	4. Identicon for signatures is shown in list on each node for user to verify
	5. Optionally: multifactor authentications are conducted to improve security further
	6. The list of nodes is updated to show the "security level" of each node

	Pairing proceeds as follows:

	As soon as a node is discovered (secure communication is established), it's
	then up to the user to verify that the signatures match her expectations by
	inspecting the respective identity information closely.

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
	QTimer										mTimer;
	quint64										mLastZooPair;
	QUrl										mServerURL;
	QSharedPointer<qhttp::client::QHttpClient>	mClient;
	QSharedPointer<Node>						mNode;
	QSharedPointer<Key>							mKey;
	bool										mLog;
	HoneymoonScheduler<quint64>					mHoneymoonScheduler;
	ConfigureHelper								mConfigureHelper;


private:
	void registerPossibleAssociate(QVariantMap map);

public:
	explicit DiscoveryClient();
	virtual ~DiscoveryClient();

public:
	void configure(QSharedPointer<Node> node
				   , quint64 triggeredInterval = 1000 // When triggered we want to make a discovery call every second
				   , quint64 idleInterval = 60000 // When not triggered we conduct a discovery once per minute
				   , quint64 triggeredTime = 20000 // Stay triggered for 20 seconds
				   , quint64 decayTime = 40000 // Decay triggering over 40 second period
				  );

	// Convenience selectors
private:
	QSharedPointer<Key> localKey() const;

public:
	void setURL(const QUrl&);
	QUrl URL() const;
	bool isActive() const;
	void activate(bool start);

	// Perform one shot discovery by invoking discovery request to discovery server
	// NOTE: This should only be invoked from internal timer.
	void discover();

	QSharedPointer<Node> node() const;

	quint64 lastZooPairTime() const;

	void setLogging(const bool logging);
	bool isLogging() const;

private slots:
	void onTimer();

signals:
	void nodeDiscovered(QString id);

};

#endif
// DISCOVERYCLIENT_HPP
