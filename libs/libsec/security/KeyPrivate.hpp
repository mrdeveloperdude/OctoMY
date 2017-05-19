#ifndef KEYPRIVATE_HPP
#define KEYPRIVATE_HPP

#include "qpolarssl/qpolarsslpki.hpp"

#include <QVariantMap>

class Key;


/*
  Qt Style private class for Key
*/
class KeyPrivate
{

private:
	Q_DISABLE_COPY(KeyPrivate)
	//		Q_DECLARE_PUBLIC(Key)

protected:

	QString mKey;
	QString mPubKey;
	QString mID;
	qpolarssl::Pki mPKI;
	static int mKCT;
	int mKID;
	bool mValidPrivate;
	bool mValidPublic;

public:
	static QString hash(QString input);

private:

	void parse(bool isPublic);
public:



	// Parse public or private key in PEM format
	explicit KeyPrivate(QVariantMap key, bool isPublic);
	// Parse public or private key in PEM format
	explicit KeyPrivate(QString key, bool isPublic);
	// Generate key-pair of given bit length
	explicit KeyPrivate(quint32 bits);
	//Empty key
	explicit KeyPrivate();

	virtual ~KeyPrivate();

	// Friendships
public:

	friend class Key;
};


#endif // KEYPRIVATE_HPP
