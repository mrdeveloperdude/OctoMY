#include "ReliabilitySystem.hpp"
#include <QDebug>

ReliabilitySystem::ReliabilitySystem( unsigned int max_sequence ):
	max_sequence(max_sequence)
  , local_sequence(0)
  , remote_sequence(0)
  , sent_packets(0)
  , recv_packets(0)
  , lost_packets(0)
  , acked_packets(0)
  , sent_bandwidth(0.0f)
  , acked_bandwidth(0.0f)
  , rtt(0.0f)
  , rtt_maximum(1.0f)
{
	reset();
}

void ReliabilitySystem::reset(){
	local_sequence = 0;
	remote_sequence = 0;
	sentQueue.clear();
	receivedQueue.clear();
	pendingAckQueue.clear();
	ackedQueue.clear();
	sent_packets = 0;
	recv_packets = 0;
	lost_packets = 0;
	acked_packets = 0;
	sent_bandwidth = 0.0f;
	acked_bandwidth = 0.0f;
	rtt = 0.0f;
	rtt_maximum = 1.0f;
}

void ReliabilitySystem::packetSent( int size ) {
	if ( sentQueue.exists( local_sequence ) ) {
		qDebug()<<"local sequence "<< local_sequence <<" exists";
		for ( PacketQueue::iterator itor = sentQueue.begin(); itor != sentQueue.end(); ++itor ){
			qDebug()<< " + "<<itor->sequence ;
		}
	}
	//			assert( !sentQueue.exists( local_sequence ) );
	//		assert( !pendingAckQueue.exists( local_sequence ) );
	PacketData data;
	data.sequence = local_sequence;
	data.time = 0.0f;
	data.size = size;
	sentQueue.push_back( data );
	pendingAckQueue.push_back( data );
	sent_packets++;
	local_sequence++;
	if ( local_sequence > max_sequence ){
		local_sequence = 0;
	}
}

void ReliabilitySystem::packetReceived( unsigned int sequence, int size ) {
	recv_packets++;
	if ( receivedQueue.exists( sequence ) ){
		return;
	}
	PacketData data;
	data.sequence = sequence;
	data.time = 0.0f;
	data.size = size;
	receivedQueue.push_back( data );
	if ( sequenceIsMoreRecent( sequence, remote_sequence, max_sequence ) ){
		remote_sequence = sequence;
	}
}

unsigned int ReliabilitySystem::generateAckBits() {
	return generateAckBits( remote_sequence, receivedQueue, max_sequence );
}

void ReliabilitySystem::processAck( unsigned int ack, unsigned int ack_bits ) {
	processAck( ack, ack_bits, pendingAckQueue, ackedQueue, acked, acked_packets, rtt, max_sequence );
}

void ReliabilitySystem::update( float deltaTime ) {
	acked.clear();
	advanceQueueTime( deltaTime );
	updateQueues();
	updateStats();
#ifdef NET_UNIT_TEST
	validate();
#endif
}

#ifdef NET_UNIT_TEST
void validate(){
	sentQueue.verify_sorted( max_sequence );
	receivedQueue.verify_sorted( max_sequence );
	pendingAckQueue.verify_sorted( max_sequence );
	ackedQueue.verify_sorted( max_sequence );
}
#endif

// utility functions

bool ReliabilitySystem::sequenceIsMoreRecent( unsigned int s1, unsigned int s2, unsigned int max_sequence ){
	return (( s1 > s2 ) && ( s1 - s2 <= max_sequence/2 )) || (( s2 > s1 ) && ( s2 - s1 > max_sequence/2 ));
}

int ReliabilitySystem::bitIndexForSequence( unsigned int sequence, unsigned int ack, unsigned int max_sequence ){
	//			assert( sequence != ack );
	//			assert( !sequence_more_recent( sequence, ack, max_sequence ) );
	if ( sequence > ack ) {
		//				assert( ack < 33 );
		//				assert( max_sequence >= sequence );
		return ack + ( max_sequence - sequence );
	}
	else {
		//				assert( ack >= 1 );
		//				assert( sequence <= ack - 1 );
		return ack - 1 - sequence;
	}
}

unsigned int ReliabilitySystem::generateAckBits( unsigned int ack, const PacketQueue & received_queue, unsigned int max_sequence ) {
	unsigned int ack_bits = 0;
	for ( PacketQueue::const_iterator itor = received_queue.begin(); itor != received_queue.end(); itor++ ) {
		if ( itor->sequence == ack || sequenceIsMoreRecent( itor->sequence, ack, max_sequence ) ){
			break;
		}
		int bit_index = bitIndexForSequence( itor->sequence, ack, max_sequence );
		if ( bit_index <= 31 ){
			ack_bits |= 1 << bit_index;
		}
	}
	return ack_bits;
}

void ReliabilitySystem::processAck( unsigned int ack
									, unsigned int ack_bits
									, PacketQueue & pending_ack_queue
									, PacketQueue & acked_queue
									, QVector<unsigned int> & acked
									, unsigned int & acked_packets
									, float & rtt
									, unsigned int max_sequence)
{
	if ( pending_ack_queue.empty() ){
		return;
	}

	PacketQueue::iterator itor = pending_ack_queue.begin();
	while ( itor != pending_ack_queue.end() ) {
		bool wasAcked = false;

		if ( itor->sequence == ack ) {
			wasAcked = true;
		}
		else if ( !sequenceIsMoreRecent( itor->sequence, ack, max_sequence ) ) {
			int bit_index = bitIndexForSequence( itor->sequence, ack, max_sequence );
			if ( bit_index <= 31 ){
				wasAcked = ( ack_bits >> bit_index ) & 1;
			}
		}

		if ( wasAcked ) {
			rtt += ( itor->time - rtt ) * 0.1f;
			acked_queue.insert_sorted( *itor, max_sequence );
			acked.push_back( itor->sequence );
			acked_packets++;
			itor = pending_ack_queue.erase( itor );
		}
		else{
			++itor;
		}
	}
}

// data accessors



unsigned int ReliabilitySystem::localSequence() const {
	return local_sequence;
}

unsigned int ReliabilitySystem::remoteSequence() const {
	return remote_sequence;
}

unsigned int ReliabilitySystem::maxSequence() const{
	return max_sequence;
}


void ReliabilitySystem::acks( unsigned int ** acked, int & count ){
	*acked = &this->acked[0];
	count = (int) this->acked.size();
}

unsigned int ReliabilitySystem::sentPackets() const{
	return sent_packets;
}

unsigned int ReliabilitySystem::receivedPackets() const{
	return recv_packets;
}

unsigned int ReliabilitySystem::lostPackets() const{
	return lost_packets;
}

unsigned int ReliabilitySystem::ackedPackets() const{
	return acked_packets;
}

float ReliabilitySystem::sentBandwidth() const{
	return sent_bandwidth;
}

float ReliabilitySystem::ackedBandwidth() const{
	return acked_bandwidth;
}

float ReliabilitySystem::roundTripTime() const{
	return rtt;
}

int ReliabilitySystem::headerSize() const{
	return 12;
}


void ReliabilitySystem::advanceQueueTime( float deltaTime ) {
	for ( PacketQueue::iterator itor = sentQueue.begin(); itor != sentQueue.end(); itor++ ){
		itor->time += deltaTime;
	}

	for ( PacketQueue::iterator itor = receivedQueue.begin(); itor != receivedQueue.end(); itor++ ){
		itor->time += deltaTime;
	}

	for ( PacketQueue::iterator itor = pendingAckQueue.begin(); itor != pendingAckQueue.end(); itor++ ){
		itor->time += deltaTime;
	}

	for ( PacketQueue::iterator itor = ackedQueue.begin(); itor != ackedQueue.end(); itor++ ){
		itor->time += deltaTime;
	}
}

void ReliabilitySystem::updateQueues(){
	const float epsilon = 0.001f;

	while ( sentQueue.size() && sentQueue.front().time > rtt_maximum + epsilon ){
		sentQueue.pop_front();
	}

	if ( receivedQueue.size() ) {
		const unsigned int latest_sequence = receivedQueue.back().sequence;
		const unsigned int minimum_sequence = latest_sequence >= 34 ? ( latest_sequence - 34 ) : max_sequence - ( 34 - latest_sequence );
		while ( receivedQueue.size() && !sequenceIsMoreRecent( receivedQueue.front().sequence, minimum_sequence, max_sequence ) ){
			receivedQueue.pop_front();
		}
	}

	while ( ackedQueue.size() && ackedQueue.front().time > rtt_maximum * 2 - epsilon ){
		ackedQueue.pop_front();
	}

	while ( pendingAckQueue.size() && pendingAckQueue.front().time > rtt_maximum + epsilon ) {
		pendingAckQueue.pop_front();
		lost_packets++;
	}
}

void ReliabilitySystem::updateStats() {
	int sent_bytes_per_second = 0;
	for ( PacketQueue::iterator itor = sentQueue.begin(); itor != sentQueue.end(); ++itor ){
		sent_bytes_per_second += itor->size;
	}
	int acked_packets_per_second = 0;
	int acked_bytes_per_second = 0;
	for ( PacketQueue::iterator itor = ackedQueue.begin(); itor != ackedQueue.end(); ++itor ) {
		if ( itor->time >= rtt_maximum ) {
			acked_packets_per_second++;
			acked_bytes_per_second += itor->size;
		}
	}
	sent_bytes_per_second /= rtt_maximum;
	acked_bytes_per_second /= rtt_maximum;
	sent_bandwidth = sent_bytes_per_second * ( 8 / 1000.0f );
	acked_bandwidth = acked_bytes_per_second * ( 8 / 1000.0f );
}

QString ReliabilitySystem::getSummary(){
	QString out="";
	out+="max_sequence="+QString::number(max_sequence);
	out+="\nlocal_sequence="+QString::number(local_sequence);
	out+="\nremote_sequence="+QString::number(remote_sequence);
	out+="\nsent_packets="+QString::number(sent_packets);
	out+="\nrecv_packets="+QString::number(recv_packets);
	out+="\nlost_packets="+QString::number(lost_packets);
	out+="\nacked_packets="+QString::number(acked_packets);
	out+="\nsent_bandwidth="+QString::number(sent_bandwidth);
	out+="\nacked_bandwidth="+QString::number(acked_bandwidth);
	out+="\nrtt="+QString::number(rtt);
	out+="\nrtt_maximum="+QString::number(rtt_maximum);
	out+="\nsentQueue="+QString::number(sentQueue.size());
	out+="\npendingAckQueue="+QString::number(pendingAckQueue.size());
	out+="\nreceivedQueue="+QString::number(receivedQueue.size());
	out+="\nackedQueue="+QString::number(ackedQueue.size());
	return out;
}

