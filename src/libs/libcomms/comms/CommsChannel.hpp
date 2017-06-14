/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#ifndef COMMSCHANNEL_HPP
#define COMMSCHANNEL_HPP

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QDateTime>
#include <QSharedPointer>
#include <QSet>

#include "ReliabilitySystem.hpp"
#include "FlowControl.hpp"

#include "../../libcore/basic/LogDestination.hpp"
#include "couriers/Courier.hpp"
#include "CommsSessionDirectory.hpp"

// It seems that 30 seconds would be a "safe" minimal UDP rate to avoid routers closing our "connection"
#define MINIMAL_PACKET_RATE 5000

#define OCTOMY_PROTOCOL_MAGIC (0x0C701111)
//#define OCTOMY_PROTOCOL_MAGIC_IDLE (OCTOMY_PROTOCOL_MAGIC+1)
#define OCTOMY_PROTOCOL_VERSION_CURRENT (1)
#define OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT (QDataStream::Qt_5_7)



class HubWindow;
class CommsSession;
class CommsSessionDirectory;
class KeyStore;

/*
Architecture:

TCP tries to pretend on behalf of the user that network traffic is a dependable
linear unbroken stream of bytes. From the user's perspective this has the
benefit of being easy to understand and use. The two major downsides however
are that:

1. This far from how a network actually works and so it is hard to make TCP work
   it has taken decades of evolution for TCP to become as good as it is today,
   but it is still limited by this fallacy

2. Some needs may actually be better met by not thinking about the network as a
   linear stream of bytes, so going the length to pretend that it is actually
   just gets in the way.

CommChannel is an API wrapping UDP in Qt in a way that allows the user to
exploit the benefits of communications over UDP by modelling closesly the
benefits in a way that hides their inherent complexities.

The CommChannel API works like this:

1. Users of the API register "couriers" that each is responsible for keeping the
   latest data fresh and ready for sending should an opportunity present itself.
   Couriers each tend to a certain type of packet with a certain priority and
   desired sending frequency. It is up to each courier to maintain it's own
   state.

2. CommChannel is in charge and decides the speed at which packets are sent and
   which couriers get their packets sent on each opportunity.

3. CommChannel may at any time send non-payload data in each packet or even
   special purpose network-only packets to sustain its operation. If there is no
   data to be sent by couriers, CommChannel may send no-op packets that
   facilitates the calculation of the network characteristics such as
   round trip time. This is done transparentyl to the couriers.


Notes:

1. CommChannel binds to a local address and port, but does not really
   discriminate from where inbound traffic arrives. All packets are treated
   equal as all packes inherently contain an identification of the source.

2. Communication between remote and agent is initiated when user presses the "connect"
   buttons in the respective user interfaces.

3.  At that point agent will attemt to contact all trusted remotes at their last
   known address untill one or more answer are received. From then on, all
   connections that did not result in valid responses will be closed, and only
   retried periodically.

4. All Agent initiated communication will be broadcast to all active remotes in
   paralell. Remote initiated transfers will remain private.


Session management strategy:

 + Uppon the start of application, all entries in NodeAssociateStore are added to CommsSessionDirectory with their stored state. This ensures that session will persist.
 + Comms channel may be in "normal" or "honeymoon" mode where honeymoon mode is typically enabled in a period just after communications are enabled, or at users disgresion.
 + Comms channel in "honeymoon" mode will ping all nodes that are not active continuously at a short (seconds) interval until the honeymoon wears off.
 + Comms channel in "normal" mode will ping all nodes that are not active at an exponentially decaying frequency as a function of how long since their last recorded ping response.
 + Any node that replies to a ping are immideately upgraded to "active"
 + Any active node has a handshake started, and communication is maintained with them until completion or until the connection times out
 + If communication with an active node is explicitly completed, it will not receive pings, even during honeymoon, for a specified grace period
 + If communication with an active node simply times out it will instead continue to parttake in the pinging as normal.
 + The node that was last active is always treated as if it is no older than 1 hour, and will in inactive periods thus be pinged as often.


	 + A packet is defined as initial when it contains a session ID of 0
	 + A packet is defined as broken when it does not adhere to the protocol by displaying a lack/excess of data or data in the wrong format
	 + A packet is defined as whole when it is not broken
	 + A packet is defined as hacked when it is whole according to comms protocol but broken according to tamper protocol

	Comms protocol is the language of OctoMY™ in network communication.
	Tamper protocol is an extra layer of protection beside comms protocol to detect attempts to tamper with communications. It has validation checks that are not necessary by the comms protocol, but add tells to the authenticity of the data (would the real implementation send data in this way)?

	A valid handshake to start a new session looks like this (similar in concept to the 3-way TCP handshake):

	1. A sends SYN to B:
		Hi B. Here is my FULL-ID + NONCE ENCODED WITH YOUR PUBKEY.

	2. B answers SYN-ACK to A:
		Hi A. HERE IS MY FULL-ID + SAME NONCE + NEW NONCE ENCODED WITH YOUR PUBKEY back atch'a.

	3. A answers ACK to B:
		Hi again B. FULL-ID + NONCES WELL RECEIVED.

	At this point session is established

	NOTE: For every step in this protocol, if A is waiting for data from B that does not arrive, it will attempt to resend it's last message on a regular interval until it does (or some error condition or other state change occurs).

	Comms have two "layers", the intrinsic layer and the courier layer.
	The courier part is reserved for the application layer that wish to conduct communication using comms channel.
	The intrinsic part is reserved for internal affairs of commschannel.
	Intrinsic and courier parts of commschannel should not depend directly on one another and their respective implementation should not be mixed.

	Intrinsic parts of comms include the following:

	 + Session management
		 + Session initiation/handshake
		 + Exchange of transmission control data
		 + Session tear-down
	 + Bandwidth management
		 + Detection of available bandwidth
		 + Throttling to avoid excessive bandwidth use
		 + Continuous monitoring and ajustment of protocol parameters optimize flow of packets (including the priority and timing of couriers)
	 + Encryption management
		 + Signing and sign verification of un-encrypted protocol text based on client RSA keypairs
		 + Generation and exchange of encryption keys based on client RSA keypairs
		 + Generation of security primitives such as nonces
		 + Encryption and decryption of protocol text
	 + Reliability management
		 + Maintaining of continued UDP connection over unreliable network components such as consumer grade routers and wireles radios with poor coverage by the dispatch of necessary communication with STUN services, or sending of antler packets.
		 + Detection and removal of duplicate and corrupt packets
		 + Detection and re-sending of missing packets
		 + Reordering of ordered packet sequences

	Please note that the the expensive and complex intrinsic features such as
	relibability and encryption of CommsChannel are invoked only when needed.

	When the amount of data needed for intrinsic features is extensive, separate
	"intrinsic packets" will be sent, while other lesser-in-size intrinsic data
	such as counters will instead accompany each packet. Protocol dictates when
	such dedicated packets will be needed or not, and changes in this part of the
	protocol should not affect the higher level courier interface.


Flags:
	+ Bit 1	"SYN"
	+ Bit 2 "ACK"
	+ Bit 3
	+ Bit 4 "Reserved for future use"
	+ Bit 5 "Reserved for future use"
	+ Bit 6 "Reserved for future use"
	+ Bit 7 "Reserved for future use"
	+ Bit 8 "Reserved for future use"



  Node

   + Agent -> AgentControls -> AgentCourierSet

   + Remote -> ClientWidget -> RemoteCourierSet

	CommsChannel - A mode of communication over UDP, utilizing Couriers to transfer different kinds of data

	CommsSession - A session of using CommsChannel. Starts as rogue, proceeds with handshake to exchange full ID and 64bit nonceID, agree on security protocols and bandwidth limits.

	CommsSessionDirectory - The list of sessions in use by the CommsChannel

	CommsSignature - [DEPRECATED, use full key->id() string instead] A special purpose identification mapping between full ID and short hand 64bit integer ID used only by CommsChannel and friends.

	NodeAssociate - Address book entry for one node. Stored in NodeAssociateStore. Meant to be persistent between invocations.

	NodeAssociateStore - Place to keep NodeAssociates

	ReliabilitySystem - Separate system to maintain reliability in the communications. Enabled when needed.

	FlowControl - Separate system to maintain flow control (apply throttling and idling to avoid constipation and stalls) in the communications. Enabled when needed.

	Courier - A class responsible for a certain part of communications over CommsChannel

	CourierSet - A collection of couriers to be handled as one.

	AgentCourierSet - Specialization of CourierSet as convenience for Agent

	RemoteCourierSet - Specialization of CourierSet as convenience for Remote

	 + A packet is defined as initial when it contains a session ID of 0
	 + A packet is defined as broken when it does not adhere to the protocol by displaying a lack/excess of data or data in the wrong format
	 + A packet is defined as whole when it is not broken
	 + A packet is defined as hacked when it is whole according to comms protocol but broken according to tamper protocol

	Comms protocol is the language of OctoMY™ in network communication.
	Tamper protocol is an extra layer of protection beside comms protocol to detect attempts to tamper with communications. It has validation checks that are not necessary by the comms protocol, but add tells to the authenticity of the data (would the real implementation send data in this way)?

	A valid handshake to start a new session looks like this (similar in concept to the 3-way TCP handshake):

	1. A sends SYN to B:
		Hi B. Here is my FULL-ID + NONCE ENCODED WITH YOUR PUBKEY.

	2. B answers SYN-ACK to A:
		Hi A. HERE IS MY FULL-ID + SAME NONCE + NEW NONCE ENCODED WITH YOUR PUBKEY back atch'a.

	3. A answers ACK to B:
		Hi again B. FULL-ID + NONCES WELL RECEIVED.

	At this point session is established

	NOTE: For every step in this protocol, if A is waiting for data from B that does not arrive, it will attempt to resend it's last message on a regular interval until it does (or some error condition or other state change occurs).

	Comms have two "layers", the intrinsic layer and the courier layer.
	The courier part is reserved for the application that wish to conduct communication using comms channel.
	The intrinsic part is reserved for internal affairs of commschannel.
	Intrinsic and courier parts of commschannel should not depend directly on one another and their respective implementation should not be mixed.

	NOTE: We have some conflicting requirements for the protocol:

	 1. The protocol should be as stateless as possible for robustness
	 2. The protocol should be as low-bandwidth as possible

	 Where is the conflict? For every mode/state you add, you save bytes going over the wire (because the other side will "remember" the state),
	 while for every mode/state you store, you will add an opportunity for failure resulting from missing state-carrying packets.

	 In other words, sendiong all state in every packet means that no state is ever dropped, but adds up to lots of data.

	 To alleviate this conflict, we do the following:

	 1. Make the protocoll statefull to save bandwidth, while
	 2. Adding a mechanism to verify the state so that mismatch can be detected and state can be re-sent

	 We call this mechanism "sync", and every packet may request sync.

	Intrinsic parts of comms include the following:

	 + Session management
		 + Session initiation/handshake
		 + Exchange of transmission control data
		 + Session tear-down
	 + Bandwidth management
		 + Detection of available bandwidth
		 + Throttling to avoid excessive bandwidth use
		 + Continuous monitoring and ajustment of protocol parameters optimize flow of packets (including the priority and timing of couriers)
	 + Encryption management
		 + Signing and sign verification of un-encrypted protocol text based on client RSA keypairs
		 + Generation and exchange of encryption keys based on client RSA keypairs
		 + Generation of security primitives such as nonces
		 + Encryption and decryption of protocol text
	 + Reliability management
		 + Maintaining of continued UDP connection over unreliable network components such as consumer grade routers and wireles radios with poor coverage by the dispatch of necessary communication with STUN services, or sending of antler packets.
		 + Detection and removal of duplicate and corrupt packets
		 + Detection and re-sending of missing packets
		 + Reordering of ordered packet sequences

	Please note that the the expensive and complex intrinsic features such as
	relibability and encryption of CommsChannel are invoked only when needed.

	When the amount of data needed for intrinsic features is extensive, separate
	"intrinsic packets" will be sent, while other lesser-in-size intrinsic data
	such as counters will instead accompany each packet. Protocol dictates when
	such dedicated packets will be needed or not, and changes in this part of the
	protocol should not affect the higher level courier interface.



	multimagic approach:

	First byte is enum/session ID:

	0-255 = reserved for enum [
	PUNCH // Idle packets meant to keep UDP sessions up. These packets are completely empty to save bandwidth
	, SYN // Start of session handshake
	, ACK // Middle of session handshake
	, SYNACK // End of session handshake

]
	256 - 64000 - valid session ID





*/




enum Multimagic {
	MULTIMAGIC_IDLE=0
	, MULTIMAGIC_SYN
	, MULTIMAGIC_SYNACK
	, MULTIMAGIC_ACK
	, MULTIMAGIC_LAST

};

#include "RateCalculator.hpp"

class PacketReadState;
class PacketSendState;
class NodeAssociateStore;

class CommsChannel : public QObject
{
	Q_OBJECT
private:
	static const quint64 CONNECTION_TIMEOUT;
	static const qint32 MAX_UDP_PAYLOAD_SIZE;

	//Receive counter used in debug messages to make sense of the order of things
	static quint32 sTotalRecCount;
	static quint32 sTotalTxCount;

private:

	QUdpSocket mUDPSocket; // The socket used by this cc to communicate with the other side
	QTimer mSendingTimer;  // The timer used to schedule when packets are sent to the other side
	KeyStore &mKeystore;   // The keystore which is used for encryption (the local key pair is used, as looked up with mLocalID)
	NodeAssociateStore &mPeers; // The store wich is used to find network addresses to use when creating new sessions
	CommsSessionDirectory mSessions; // The directory of sessions for this cc
	QList<Courier *> mCouriers; // The couriers that are in use by this cc. Only active couriers are in this list for performance reasons.
	QMap<quint32, Courier *> mCouriersByID; // Map for quickly finding a particular courier by it's unique ID
	quint64 mLocalSessionID;
	NetworkAddress mLocalAddress;

	RateCalculator mRXRate;
	RateCalculator mTXRate;
	RateCalculator mTXOpportunityRate;
	RateCalculator mTXScheduleRate;

	bool mConnected;
	// When honeymoon mode is enabled, all inactive associates are pinged continuously in an effort to start new connections
	bool mHoneyMoon;

	QMap<quint64, Courier *> mSchedule; // Couriers with priority at next sending oportunity as calculated by rescheduleSending()
	qint64 mMostUrgentCourier; // The number of milliseconds until next sending opportunity as calculated by rescheduleSending()



public:

	explicit CommsChannel(KeyStore &keystore, NodeAssociateStore &peers, QObject *parent=nullptr);
	explicit CommsChannel(KeyStore &keystore, QObject *parent=nullptr);

private:

	void detectConnectionChanges(const quint64 now);
	QSharedPointer<CommsSession>  lookUpSession(QString id, SESSION_ID_TYPE desiredRemoteSessionID);

public:

	CommsSessionDirectory &sessions();
	void start(NetworkAddress localAddress);
	void stop();
	bool isStarted() const;
	bool isConnected() const;

	NetworkAddress localAddress();
	QString localID();

	QString getSummary();
	//void setID(const QString &id);
	void setHookCommsSignals(QObject &ob, bool hook);
	void setHookCourierSignals(Courier &courier, bool hook);
	void setCourierRegistered(Courier &, bool);
	int courierCount();
	bool hasCourier(Courier &c) const;
	Courier *getCourierByID(quint32 id) const;

	qint64 sendRawData(QByteArray datagram, NetworkAddress address);

private:


	void appendLog(QString);


	void doSend( PacketSendState &state);

	void sendIdle(const quint64 &now, QSharedPointer<CommsSession> session);
	void sendSyn(PacketSendState &state);
	void sendSynAck(PacketSendState &state);
	void sendAck(PacketSendState &state);
	void sendData(const quint64 &now, Courier &courier);

	// CommsChannel signals
signals:
	//		void receivePacket(QSharedPointer<QDataStream> data,QHostAddress host, quint16 port);
	void commsError(QString message);
	void commsClientAdded(CommsSession *c);
	void commsConnectionStatusChanged(bool c);

public slots:
	// Re-calculate the schedule for sending timer. May result in sending timer being called at a different time then what it was scheduled for when this method was called
	// You should call this whenever there is a chance the schedule might have changed
	quint64 rescheduleSending(quint64 now);


private:

	bool recieveEncryptedBody(PacketReadState &state);
	bool recieveMagicAndVersion(PacketReadState &state);

	void recieveIdle(PacketReadState &state);
	void recieveSyn(PacketReadState &state);
	void recieveSynAck(PacketReadState &state);
	void recieveAck(PacketReadState &state);
	void recieveData(PacketReadState &state);

// Send & receive slots
private slots:

	void receivePacketRaw(QByteArray ba,QHostAddress host, quint16 port);
	void onSendingTimer();

public slots:

	void onReadyRead();
	void onUdpError(QAbstractSocket::SocketError);
};



#endif // COMMSCHANNEL_HPP
