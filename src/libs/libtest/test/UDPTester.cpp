#include "UDPTester.hpp"

UDPTester::UDPTester(NetworkAddress fromAddr, NetworkAddress toAddr)
	: mFromAddr(fromAddr)
	, mToAddr(toAddr)
{
	const bool success = mSocket.bind(mFromAddr.ip(), mFromAddr.port());
	qDebug()<<"Data sender bound to"<< mFromAddr.toString()<< (success?" succeeded": (" failed with '"+mSocket.errorString()+"'") );
}

qint64 UDPTester::send(QByteArray data)
{
	if(!mSocket.isWritable()) {
		qWarning()<<"Socket not writable";
	}
	const qint64 bytes=mSocket.writeDatagram(data, mToAddr.ip(), mToAddr.port());
	if(bytes<0) {
		qWarning()<<"Error senfing datagram. errorString="<<mSocket.errorString();
	}
	if(bytes!= data.size()) {
		qWarning()<<"Could not send complete datagram. errorString="<<mSocket.errorString();
	}
	return bytes;
}

QByteArray UDPTester::receive()
{
	if(!mSocket.isReadable()) {
		qWarning()<<"Socket not readable";
	}
	QByteArray rxDatagram;
	rxDatagram.resize(mSocket.pendingDatagramSize());
	QHostAddress rxHost;
	quint16 rxPort=0;
	const qint64 bytes=mSocket.readDatagram(rxDatagram.data(), rxDatagram.size(), &rxHost, &rxPort);
	if(bytes< 0) {
		qWarning()<<"Error receiving datagram. errorString="<<mSocket.errorString();
	}
	return rxDatagram;

}
