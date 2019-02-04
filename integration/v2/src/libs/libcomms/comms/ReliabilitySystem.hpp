#ifndef RELIABILITYSYSTEM_HPP
#define RELIABILITYSYSTEM_HPP

#include <QVector>

struct PacketData {
	quint32 sequence;			// packet sequence number
	float time;					    // time offset since packet was sent or received (depending on context)
	qint32 size;						// packet size in bytes
};


class PacketQueue : public QList<PacketData>
{
public:

	bool exists( quint32 sequence );

	void insertSorted( const PacketData & p, quint32 max_sequence );
#ifdef NET_UNIT_TEST
	void verify_sorted( quint32 max_sequence );
#endif
};



class ReliabilitySystem
{
private:

	quint32 mMaxSequence;			// maximum sequence value before wrap around (used to test sequence wrap at low # values)
	quint32 mLocalSequence;		// local sequence number for most recently sent packet
	quint32 mRemoteSequence;		// remote sequence number for most recently received packet

	quint32 mSentPackets;			// total number of packets sent
	quint32 mReceivedPackets;			// total number of packets received
	quint32 mLostPackets;			// total number of packets lost
	quint32 mAckedPackets;			// total number of packets acked

	float mSentBandwidth;				// approximate sent bandwidth over the last second
	float mAckedBandwidth;				// approximate acked bandwidth over the last second
	float mRTT;							// estimated round trip time
	float mRTTMaximum;					// maximum expected round trip time (hard coded to one second for the moment)

	QVector<quint32> mAcked;		// acked packets from last set of packet receives. cleared each update!

	PacketQueue mSentQueue;				// sent packets used to calculate sent bandwidth (kept until rtt_maximum)
	PacketQueue mPendingAckQueue;		// sent packets which have not been acked yet (kept until rtt_maximum * 2 )
	PacketQueue mReceivedQueue;			// received packets for determining acks to send (kept up to most recent recv sequence - 32)
	PacketQueue mAckedQueue;				// acked packets (kept until rtt_maximum * 2)
public:

	ReliabilitySystem( quint32 mMaxSequence = 0xFFFFFFFF );
	void reset();
	void packetSent( qint32 size ) ;
	void packetReceived( quint32 sequence, qint32 size ) ;
	quint32 generateAckBits() ;
	void processAck( quint32 ack, quint32 ack_bits );
	void update( float deltaTime ) ;

#ifdef NET_UNIT_TEST
	void validate();
#endif

	// utility functions

	static bool sequenceIsMoreRecent( quint32 s1, quint32 s2, quint32 mMaxSequence );
	static qint32 bitIndexForSequence( quint32 sequence, quint32 ack, quint32 mMaxSequence );
	static quint32 generateAckBits( quint32 ack, const PacketQueue & received_queue, quint32 mMaxSequence );
	static void processAck( quint32 ack
							, quint32 ack_bits
							, PacketQueue & pending_ack_queue
							, PacketQueue & acked_queue
							, QVector<quint32> & acks
							, quint32 & mAckedPackets
							, float & mRTT
							, quint32 mMaxSequence);


	quint32 localSequence() const;
	quint32 remoteSequence() const ;
	quint32 maxSequence() const;
	void acks( quint32 ** acks, qint32 & count );
	quint32 sentPackets() const;
	quint32 receivedPackets() const;
	quint32 lostPackets() const;
	quint32 ackedPackets() const;
	float sentBandwidth() const;
	float ackedBandwidth() const;
	float roundTripTime() const;
	qint32 headerSize() const;

protected:

	void advanceQueueTime( float deltaTime ) ;
	void updateQueues();
	void updateStats() ;

public:

	QString toString();

};

#endif // RELIABILITYSYSTEM_HPP
