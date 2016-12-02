#ifndef RELIABILITYSYSTEM_HPP
#define RELIABILITYSYSTEM_HPP

#include <QVector>




struct PacketData{
		quint32 sequence;			// packet sequence number
		float time;					    // time offset since packet was sent or received (depending on context)
		qint32 size;						// packet size in bytes
};



class PacketQueue : public QList<PacketData>{
	public:

		bool exists( quint32 sequence );

		void insertSorted( const PacketData & p, quint32 max_sequence );
#ifdef NET_UNIT_TEST
		void verify_sorted( quint32 max_sequence );
#endif
};



class ReliabilitySystem{
	private:

		quint32 max_sequence;			// maximum sequence value before wrap around (used to test sequence wrap at low # values)
		quint32 local_sequence;		// local sequence number for most recently sent packet
		quint32 remote_sequence;		// remote sequence number for most recently received packet

		quint32 sent_packets;			// total number of packets sent
		quint32 recv_packets;			// total number of packets received
		quint32 lost_packets;			// total number of packets lost
		quint32 acked_packets;			// total number of packets acked

		float sent_bandwidth;				// approximate sent bandwidth over the last second
		float acked_bandwidth;				// approximate acked bandwidth over the last second
		float rtt;							// estimated round trip time
		float rtt_maximum;					// maximum expected round trip time (hard coded to one second for the moment)

		QVector<quint32> acked;		// acked packets from last set of packet receives. cleared each update!

		PacketQueue sentQueue;				// sent packets used to calculate sent bandwidth (kept until rtt_maximum)
		PacketQueue pendingAckQueue;		// sent packets which have not been acked yet (kept until rtt_maximum * 2 )
		PacketQueue receivedQueue;			// received packets for determining acks to send (kept up to most recent recv sequence - 32)
		PacketQueue ackedQueue;				// acked packets (kept until rtt_maximum * 2)
	public:

		ReliabilitySystem( quint32 max_sequence = 0xFFFFFFFF );
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

		static bool sequenceIsMoreRecent( quint32 s1, quint32 s2, quint32 max_sequence );
		static qint32 bitIndexForSequence( quint32 sequence, quint32 ack, quint32 max_sequence );
		static quint32 generateAckBits( quint32 ack, const PacketQueue & received_queue, quint32 max_sequence );
		static void processAck( quint32 ack
								 , quint32 ack_bits
								 , PacketQueue & pending_ack_queue
								 , PacketQueue & acked_queue
								 , QVector<quint32> & acks
								 , quint32 & acked_packets
								 , float & rtt
								 , quint32 max_sequence);


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

		QString getSummary();

};

#endif // RELIABILITYSYSTEM_HPP
