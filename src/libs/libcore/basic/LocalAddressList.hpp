#ifndef LOCALADDRESSLIST_HPP
#define LOCALADDRESSLIST_HPP

#include <QList>
#include <QHostAddress>

class LocalAddressList: public QList<QHostAddress>
{
private:
	quint16 mPort;
	int mSelectedAddress;
public:
	LocalAddressList(quint16 port=0);

public:

	void setPort(quint16 port);
	quint16 port() const;



	void setCurrent(QHostAddress address, quint16 port);
	QHostAddress currentAddress() const;

	void updateAddresses();
};

#endif // LOCALADDRESSLIST_HPP
