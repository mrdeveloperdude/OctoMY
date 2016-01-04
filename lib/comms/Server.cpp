#include "Server.hpp"

#include "basic/Standard.hpp"
#include "basic/ServerWindow.hpp"
#include "Client.hpp"

#include <QDataStream>
#include <QDateTime>



inline bool sequence_more_recent( unsigned int s1, unsigned int s2, unsigned int max_sequence ){
	return (( s1 > s2 ) && ( s1 - s2 <= max_sequence/2 )) || (( s2 > s1 ) && ( s2 - s1 > max_sequence/2 ));
}



bool PacketQueue::exists( unsigned int sequence ){
	for ( iterator itor = begin(); itor != end(); ++itor ){
		if ( itor->sequence == sequence ){
			return true;
		}
	}
	return false;
}

void PacketQueue::insert_sorted( const PacketData & p, unsigned int max_sequence ){
	if ( empty() ){
		push_back( p );
	}
	else {
		if ( !sequence_more_recent( p.sequence, front().sequence, max_sequence ) ){
			push_front( p );
		}
		else if ( sequence_more_recent( p.sequence, back().sequence, max_sequence ) ){
			push_back( p );
		}
		else {
			for ( PacketQueue::iterator itor = begin(); itor != end(); itor++ ){
				//assert( itor->sequence != p.sequence );
				if ( sequence_more_recent( itor->sequence, p.sequence, max_sequence ) ){
					insert( itor, p );
					break;
				}
			}
		}
	}
}
#ifdef NET_UNIT_TEST
void PacketQueue::verify_sorted( unsigned int max_sequence ){
	PacketQueue::iterator prev = end();
	for ( PacketQueue::iterator itor = begin(); itor != end(); itor++ ){
		assert( itor->sequence <= max_sequence );
		if ( prev != end() ){
			assert( sequence_more_recent( itor->sequence, prev->sequence, max_sequence ) );
			prev = itor;
		}
	}
}
#endif


Server::Server(LogDestination *mw):
	QObject(0)
  , udpSocket(this)
  , idleTimer(this)
  , mw(mw)
{
	if(!connect(&udpSocket, SIGNAL(readyRead()),this, SLOT(onReadyRead()),WWCONTYPE)){
		qWarning()<<"Could not connect";
	}
	qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
	if(!connect(&udpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onUdpError(QAbstractSocket::SocketError)),WWCONTYPE)){
		qWarning()<<"Could not connect";
	}
	idleTimer.setInterval(TIMEOUT_TRESHOLD*500);
	idleTimer.setTimerType(Qt::PreciseTimer);
	if(!connect(&idleTimer, SIGNAL(timeout()),this,SLOT(onIdleTimer()),WWCONTYPE)){
		qWarning()<<"Could not connect";
	}
	idleTimer.start();
}


void Server::start(QHostAddress localAddress, quint16 localPort){
	this->localAddress=localAddress;
	this->localPort=localPort;
	bool b = udpSocket.bind(localAddress, localPort);
	qDebug()<<"Binding to UDP socket for "<< localAddress <<":"<< localPort<< (b?" succeeded": " failed");
}

void Server::stop(){
	udpSocket.close();
	qDebug()<<"Closing UDP socket for "<< localAddress <<":"<< localPort;
}

void Server::setLogOutput(LogDestination *mw){
	this->mw=mw;
}

bool Server::sendPackage( QByteArray ba, QHostAddress host, quint16 port){
	Client *nl=getClient(host,port);
	if(0!=nl){
		QByteArray datagram;
		QDataStream ds(&datagram, QIODevice::WriteOnly);
		ds<<nl->reliabilitySystem.localSequence();
		ds<<nl->reliabilitySystem.remoteSequence();
		ds<<nl->reliabilitySystem.generateAckBits();
		ds<<ba;
		if(datagram.size()>512){
			qWarning()<<"WARNING: UDP packet size exceeded 512 bytes";
		}
		const qint64 written=udpSocket.writeDatagram(datagram,host,port);
		if(written<0){
			qDebug()<<"ERROR WRITING TO UDP SOCKET:"<<udpSocket.errorString();
			return false;
		}
		else{
			qDebug()<<"WROTE "<<written<<" BYTES TO UDP SOCKET";
		}
		nl->send(written);
		return true;
	}
	return false;
}

void Server::receivePacketRaw( QByteArray datagram, QHostAddress host , quint16 port ){
	qDebug()<<"RECEIVED RAW '"<<datagram<<"'' FROM "<<host<<":"<<port;
	Client *nl=getClient(host,port);
	if(0!=nl){
		QDataStream ds(&datagram, QIODevice::ReadOnly);
		const int header = 12;
		const int size=datagram.size();
		if ( size <= header ){
			emit error("Message too short: " + datagram);
			return;
		}
		unsigned int packet_sequence = 0;
		unsigned int packet_ack = 0;
		unsigned int packet_ack_bits = 0;
		ds >> packet_sequence;
		ds >> packet_ack;
		ds >> packet_ack_bits;
		QByteArray ba;
		ds >> ba;
		nl->reliabilitySystem.packetReceived( packet_sequence, size- header );
		nl->reliabilitySystem.processAck( packet_ack, packet_ack_bits );
		nl->receive();
		emit receivePacket(ba,host, port);
	}
}


QString Server::getSummary(){
	int connected=0;
	int disconnected=0;
	for (QMap<quint64, Client *>::iterator i = clients.begin(), e=clients.end(); i != e; ++i){
		const Client *nl=i.value();
		if(0!=nl){
			if(nl->connected){
				connected++;
			}
			else{
				disconnected++;
			}
		}
	}
	QString out;
	QTextStream ts(&out);
	ts<<connected<<" of " << (disconnected+connected) <<" connected\n";
	return out;
}

QMap<quint64, Client *> &Server::getClients (){
	return clients;
}

//Slot called when data arrives on socket
void Server::onReadyRead(){
	while (udpSocket.hasPendingDatagrams()){
		QByteArray datagram;
		datagram.resize(udpSocket.pendingDatagramSize());
		QHostAddress host;
		quint16 port=0;
		udpSocket.readDatagram(datagram.data(), datagram.size(),&host, &port);
		receivePacketRaw(datagram,host,port);
	}
}


void Server::onUdpError(QAbstractSocket::SocketError){
	emit error(udpSocket.errorString());
}


void Server::appendLog(QString msg){
	if(0!=mw){
		mw->appendLog(msg);
	}
}

Client *Server::getClient(QHostAddress host, quint16 port){
	const quint64 hash=Client::generateHash(host,port);
	QMap<quint64, Client *> ::const_iterator it=clients.find(hash);
	return clients.end()==it?clients.insert(hash, new Client(host,port, mw)).value():it.value();
}


void Server::onIdleTimer(){
	QByteArray ba;
	ba.resize(12);
	for (QMap<quint64, Client *>::iterator i = clients.begin(), e=clients.end(); i != e; ++i){
		Client *nl=i.value();
		if(nl->idle()){
			sendPackage(ba, nl->host, nl->port);
		}
	}

#ifdef SHOW_ACKS
	// show packets that were acked this frame
	unsigned int * acks = NULL;
	int ack_count = 0;
	reliabilitySystem.GetAcks( &acks, ack_count );
	if ( ack_count > 0 ){
		printf( "acks: %d", acks[0] );
		for ( int i = 1; i < ack_count; ++i ){
			printf( ",%d", acks[i] );
		}
		printf( "\n" );
	}
#endif

}
