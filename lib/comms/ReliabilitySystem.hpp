#ifndef RELIABILITYSYSTEM_HPP
#define RELIABILITYSYSTEM_HPP

#include <QVector>



struct PacketData{
		unsigned int sequence;			// packet sequence number
		float time;					    // time offset since packet was sent or received (depending on context)
		int size;						// packet size in bytes
};


class PacketQueue : public QList<PacketData>{
	public:

		bool exists( unsigned int sequence );

		void insertSorted( const PacketData & p, unsigned int max_sequence );
#ifdef NET_UNIT_TEST
		void verify_sorted( unsigned int max_sequence );
#endif
};




class ReliabilitySystem{
	private:

		unsigned int max_sequence;			// maximum sequence value before wrap around (used to test sequence wrap at low # values)
		unsigned int local_sequence;		// local sequence number for most recently sent packet
		unsigned int remote_sequence;		// remote sequence number for most recently received packet

		unsigned int sent_packets;			// total number of packets sent
		unsigned int recv_packets;			// total number of packets received
		unsigned int lost_packets;			// total number of packets lost
		unsigned int acked_packets;			// total number of packets acked

		float sent_bandwidth;				// approximate sent bandwidth over the last second
		float acked_bandwidth;				// approximate acked bandwidth over the last second
		float rtt;							// estimated round trip time
		float rtt_maximum;					// maximum expected round trip time (hard coded to one second for the moment)

		QVector<unsigned int> acked;		// acked packets from last set of packet receives. cleared each update!

		PacketQueue sentQueue;				// sent packets used to calculate sent bandwidth (kept until rtt_maximum)
		PacketQueue pendingAckQueue;		// sent packets which have not been acked yet (kept until rtt_maximum * 2 )
		PacketQueue receivedQueue;			// received packets for determining acks to send (kept up to most recent recv sequence - 32)
		PacketQueue ackedQueue;				// acked packets (kept until rtt_maximum * 2)
	public:

		ReliabilitySystem( unsigned int max_sequence = 0xFFFFFFFF );
		void reset();
		void packetSent( int size ) ;
		void packetReceived( unsigned int sequence, int size ) ;
		unsigned int generateAckBits() ;
		void processAck( unsigned int ack, unsigned int ack_bits );
		void update( float deltaTime ) ;

#ifdef NET_UNIT_TEST
		void validate();
#endif

		// utility functions

		static bool sequenceIsMoreRecent( unsigned int s1, unsigned int s2, unsigned int max_sequence );
		static int bitIndexForSequence( unsigned int sequence, unsigned int ack, unsigned int max_sequence );
		static unsigned int generateAckBits( unsigned int ack, const PacketQueue & received_queue, unsigned int max_sequence );
		static void processAck( unsigned int ack
								 , unsigned int ack_bits
								 , PacketQueue & pending_ack_queue
								 , PacketQueue & acked_queue
								 , QVector<unsigned int> & acks
								 , unsigned int & acked_packets
								 , float & rtt
								 , unsigned int max_sequence);


		unsigned int localSequence() const;
		unsigned int remoteSequence() const ;
		unsigned int maxSequence() const;
		void acks( unsigned int ** acks, int & count );
		unsigned int sentPackets() const;
		unsigned int receivedPackets() const;
		unsigned int lostPackets() const;
		unsigned int ackedPackets() const;
		float sentBandwidth() const;
		float ackedBandwidth() const;
		float roundTripTime() const;
		int headerSize() const;

	protected:

		void advanceQueueTime( float deltaTime ) ;
		void updateQueues();
		void updateStats() ;

	public:

		QString getSummary();

};

#endif // RELIABILITYSYSTEM_HPP
