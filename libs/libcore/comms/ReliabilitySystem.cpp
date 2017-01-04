#include "ReliabilitySystem.hpp"
#include <QDebug>



inline bool sequenceMoreRecent( quint32 s1, quint32 s2, quint32 max_sequence )
{
	return (( s1 > s2 ) && ( s1 - s2 <= max_sequence/2 )) || (( s2 > s1 ) && ( s2 - s1 > max_sequence/2 ));
}



bool PacketQueue::exists( quint32 sequence )
{
	for ( iterator itor = begin(); itor != end(); ++itor ) {
		if ( itor->sequence == sequence ) {
			return true;
		}
	}
	return false;
}

void PacketQueue::insertSorted( const PacketData & p, quint32 max_sequence )
{
	if ( empty() ) {
		push_back( p );
	} else {
		if ( !sequenceMoreRecent( p.sequence, front().sequence, max_sequence ) ) {
			push_front( p );
		} else if ( sequenceMoreRecent( p.sequence, back().sequence, max_sequence ) ) {
			push_back( p );
		} else {
			for ( PacketQueue::iterator itor = begin(); itor != end(); itor++ ) {
				//assert( itor->sequence != p.sequence );
				if ( sequenceMoreRecent( itor->sequence, p.sequence, max_sequence ) ) {
					insert( itor, p );
					break;
				}
			}
		}
	}
}
#ifdef NET_UNIT_TEST
void PacketQueue::verify_sorted( quint32 max_sequence )
{
	PacketQueue::iterator prev = end();
	for ( PacketQueue::iterator itor = begin(); itor != end(); itor++ ) {
		assert( itor->sequence <= max_sequence );
		if ( prev != end() ) {
			assert( sequence_more_recent( itor->sequence, prev->sequence, max_sequence ) );
			prev = itor;
		}
	}
}
#endif




ReliabilitySystem::ReliabilitySystem( quint32 max_sequence ):
	mMaxSequence(max_sequence)
	, mLocalSequence(0)
	, mRemoteSequence(0)
	, mSentPackets(0)
	, mReceivedPackets(0)
	, mLostPackets(0)
	, mAckedPackets(0)
	, mSentBandwidth(0.0f)
	, mAckedBandwidth(0.0f)
	, mRTT(0.0f)
	, mRTTMaximum(1.0f)
{
	reset();
}

void ReliabilitySystem::reset()
{
	mLocalSequence = 0;
	mRemoteSequence = 0;
	mSentQueue.clear();
	mReceivedQueue.clear();
	mPendingAckQueue.clear();
	mAckedQueue.clear();
	mSentPackets = 0;
	mReceivedPackets = 0;
	mLostPackets = 0;
	mAckedPackets = 0;
	mSentBandwidth = 0.0f;
	mAckedBandwidth = 0.0f;
	mRTT = 0.0f;
	mRTTMaximum = 1.0f;
}

void ReliabilitySystem::packetSent( qint32 size )
{
	if ( mSentQueue.exists( mLocalSequence ) ) {
		qDebug()<<"local sequence "<< mLocalSequence <<" exists";
		for ( PacketQueue::iterator itor = mSentQueue.begin(); itor != mSentQueue.end(); ++itor ) {
			qDebug()<< " + "<<itor->sequence ;
		}
	}
	//			assert( !sentQueue.exists( local_sequence ) );
	//		assert( !pendingAckQueue.exists( local_sequence ) );
	PacketData data;
	data.sequence = mLocalSequence;
	data.time = 0.0f;
	data.size = size;
	mSentQueue.push_back( data );
	mPendingAckQueue.push_back( data );
	mSentPackets++;
	mLocalSequence++;
	if ( mLocalSequence > mMaxSequence ) {
		mLocalSequence = 0;
	}
}

void ReliabilitySystem::packetReceived( quint32 sequence, qint32 size )
{
	mReceivedPackets++;
	if ( mReceivedQueue.exists( sequence ) ) {
		return;
	}
	PacketData data;
	data.sequence = sequence;
	data.time = 0.0f;
	data.size = size;
	mReceivedQueue.push_back( data );
	if ( sequenceIsMoreRecent( sequence, mRemoteSequence, mMaxSequence ) ) {
		mRemoteSequence = sequence;
	}
}

quint32 ReliabilitySystem::generateAckBits()
{
	return generateAckBits( mRemoteSequence, mReceivedQueue, mMaxSequence );
}

void ReliabilitySystem::processAck( quint32 ack, quint32 ack_bits )
{
	processAck( ack, ack_bits, mPendingAckQueue, mAckedQueue, mAcked, mAckedPackets, mRTT, mMaxSequence );
}

void ReliabilitySystem::update( float deltaTime )
{
	mAcked.clear();
	advanceQueueTime( deltaTime );
	updateQueues();
	updateStats();
#ifdef NET_UNIT_TEST
	validate();
#endif
}

#ifdef NET_UNIT_TEST
void validate()
{
	sentQueue.verify_sorted( max_sequence );
	receivedQueue.verify_sorted( max_sequence );
	pendingAckQueue.verify_sorted( max_sequence );
	ackedQueue.verify_sorted( max_sequence );
}
#endif

// utility functions

bool ReliabilitySystem::sequenceIsMoreRecent( quint32 s1, quint32 s2, quint32 max_sequence )
{
	return (( s1 > s2 ) && ( s1 - s2 <= max_sequence/2 )) || (( s2 > s1 ) && ( s2 - s1 > max_sequence/2 ));
}

qint32 ReliabilitySystem::bitIndexForSequence( quint32 sequence, quint32 ack, quint32 max_sequence )
{
	//			assert( sequence != ack );
	//			assert( !sequence_more_recent( sequence, ack, max_sequence ) );
	if ( sequence > ack ) {
		//				assert( ack < 33 );
		//				assert( max_sequence >= sequence );
		return ack + ( max_sequence - sequence );
	} else {
		//				assert( ack >= 1 );
		//				assert( sequence <= ack - 1 );
		return ack - 1 - sequence;
	}
}

quint32 ReliabilitySystem::generateAckBits( quint32 ack, const PacketQueue & received_queue, quint32 max_sequence )
{
	quint32 ack_bits = 0;
	for ( PacketQueue::const_iterator itor = received_queue.begin(); itor != received_queue.end(); itor++ ) {
		if ( itor->sequence == ack || sequenceIsMoreRecent( itor->sequence, ack, max_sequence ) ) {
			break;
		}
		qint32 bit_index = bitIndexForSequence( itor->sequence, ack, max_sequence );
		if ( bit_index <= 31 ) {
			ack_bits |= 1 << bit_index;
		}
	}
	return ack_bits;
}

void ReliabilitySystem::processAck( quint32 ack
									, quint32 ack_bits
									, PacketQueue & pending_ack_queue
									, PacketQueue & acked_queue
									, QVector<quint32> & acked
									, quint32 & acked_packets
									, float & rtt
									, quint32 max_sequence)
{
	if ( pending_ack_queue.empty() ) {
		return;
	}

	PacketQueue::iterator itor = pending_ack_queue.begin();
	while ( itor != pending_ack_queue.end() ) {
		bool wasAcked = false;

		if ( itor->sequence == ack ) {
			wasAcked = true;
		} else if ( !sequenceIsMoreRecent( itor->sequence, ack, max_sequence ) ) {
			qint32 bit_index = bitIndexForSequence( itor->sequence, ack, max_sequence );
			if ( bit_index <= 31 ) {
				wasAcked = ( ack_bits >> bit_index ) & 1;
			}
		}

		if ( wasAcked ) {
			rtt += ( itor->time - rtt ) * 0.1f;
			acked_queue.insertSorted( *itor, max_sequence );
			acked.push_back( itor->sequence );
			acked_packets++;
			itor = pending_ack_queue.erase( itor );
		} else {
			++itor;
		}
	}
}

// data accessors



quint32 ReliabilitySystem::localSequence() const
{
	return mLocalSequence;
}

quint32 ReliabilitySystem::remoteSequence() const
{
	return mRemoteSequence;
}

quint32 ReliabilitySystem::maxSequence() const
{
	return mMaxSequence;
}


void ReliabilitySystem::acks( quint32 ** acked, qint32 & count )
{
	*acked = &this->mAcked[0];
	count = (qint32) this->mAcked.size();
}

quint32 ReliabilitySystem::sentPackets() const
{
	return mSentPackets;
}

quint32 ReliabilitySystem::receivedPackets() const
{
	return mReceivedPackets;
}

quint32 ReliabilitySystem::lostPackets() const
{
	return mLostPackets;
}

quint32 ReliabilitySystem::ackedPackets() const
{
	return mAckedPackets;
}

float ReliabilitySystem::sentBandwidth() const
{
	return mSentBandwidth;
}

float ReliabilitySystem::ackedBandwidth() const
{
	return mAckedBandwidth;
}

float ReliabilitySystem::roundTripTime() const
{
	return mRTT;
}

qint32 ReliabilitySystem::headerSize() const
{
	return 12;
}


void ReliabilitySystem::advanceQueueTime( float deltaTime )
{
	for ( PacketQueue::iterator itor = mSentQueue.begin(); itor != mSentQueue.end(); itor++ ) {
		itor->time += deltaTime;
	}

	for ( PacketQueue::iterator itor = mReceivedQueue.begin(); itor != mReceivedQueue.end(); itor++ ) {
		itor->time += deltaTime;
	}

	for ( PacketQueue::iterator itor = mPendingAckQueue.begin(); itor != mPendingAckQueue.end(); itor++ ) {
		itor->time += deltaTime;
	}

	for ( PacketQueue::iterator itor = mAckedQueue.begin(); itor != mAckedQueue.end(); itor++ ) {
		itor->time += deltaTime;
	}
}

void ReliabilitySystem::updateQueues()
{
	const float epsilon = 0.001f;

	while ( mSentQueue.size() && mSentQueue.front().time > mRTTMaximum + epsilon ) {
		mSentQueue.pop_front();
	}

	if ( mReceivedQueue.size() ) {
		const quint32 latest_sequence = mReceivedQueue.back().sequence;
		const quint32 minimum_sequence = latest_sequence >= 34 ? ( latest_sequence - 34 ) : mMaxSequence - ( 34 - latest_sequence );
		while ( mReceivedQueue.size() && !sequenceIsMoreRecent( mReceivedQueue.front().sequence, minimum_sequence, mMaxSequence ) ) {
			mReceivedQueue.pop_front();
		}
	}

	while ( mAckedQueue.size() && mAckedQueue.front().time > mRTTMaximum * 2 - epsilon ) {
		mAckedQueue.pop_front();
	}

	while ( mPendingAckQueue.size() && mPendingAckQueue.front().time > mRTTMaximum + epsilon ) {
		mPendingAckQueue.pop_front();
		mLostPackets++;
	}
}

void ReliabilitySystem::updateStats()
{
	qint32 sent_bytes_per_second = 0;
	for ( PacketQueue::iterator itor = mSentQueue.begin(); itor != mSentQueue.end(); ++itor ) {
		sent_bytes_per_second += itor->size;
	}
	qint32 acked_packets_per_second = 0;
	qint32 acked_bytes_per_second = 0;
	for ( PacketQueue::iterator itor = mAckedQueue.begin(); itor != mAckedQueue.end(); ++itor ) {
		if ( itor->time >= mRTTMaximum ) {
			acked_packets_per_second++;
			acked_bytes_per_second += itor->size;
		}
	}
	sent_bytes_per_second /= mRTTMaximum;
	acked_bytes_per_second /= mRTTMaximum;
	mSentBandwidth = sent_bytes_per_second * ( 8 / 1000.0f );
	mAckedBandwidth = acked_bytes_per_second * ( 8 / 1000.0f );
}

QString ReliabilitySystem::toString()
{
	QString out="";
	out+="max_sequence="+QString::number(mMaxSequence);
	out+="\nlocal_sequence="+QString::number(mLocalSequence);
	out+="\nremote_sequence="+QString::number(mRemoteSequence);
	out+="\nsent_packets="+QString::number(mSentPackets);
	out+="\nrecv_packets="+QString::number(mReceivedPackets);
	out+="\nlost_packets="+QString::number(mLostPackets);
	out+="\nacked_packets="+QString::number(mAckedPackets);
	out+="\nsent_bandwidth="+QString::number(mSentBandwidth);
	out+="\nacked_bandwidth="+QString::number(mAckedBandwidth);
	out+="\nrtt="+QString::number(mRTT);
	out+="\nrtt_maximum="+QString::number(mRTTMaximum);
	out+="\nsentQueue="+QString::number(mSentQueue.size());
	out+="\npendingAckQueue="+QString::number(mPendingAckQueue.size());
	out+="\nreceivedQueue="+QString::number(mReceivedQueue.size());
	out+="\nackedQueue="+QString::number(mAckedQueue.size());
	return out;
}
