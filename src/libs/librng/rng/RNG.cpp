#include "RNG.hpp"

#include <QDataStream>

//Ctor
RNG::RNG(QString name)
	:name (name)
{

}

RNG::~RNG()
{
}

// Fallback implementation that continuously re-seeding using random output combined with key input
inline void RNG::init(quint32 init_key[], size_t key_length)
{
	//qDebug()<<"init: ---------"<<key_length;
	//Start with "clean slate"
	init();
	//Follow up with re-seeding using random output combined with key input
	if (key_length > 0) {
		for(size_t i=0; i<key_length; ++i) {
			quint32 k=init_key[i];
			quint32 v=generateInt32();
			quint32 s=(k+1)^v;
			//qDebug()<<"init: "<<k<<v<<s;
			init(s);
		}
	}

}

static QVector<quint32> byteArrayToIntVector(QByteArray ba)
{
	const int sz = ba.size();
	const int bs=sizeof(quint32);
	const int isz = (( sz + (bs-1) ) % bs);
	const int nsz=isz*bs;
	ba.resize(nsz);
	QDataStream ds(ba);
	QVector<quint32> v;
	for(int i=0; i<isz; ++i) {
		quint32 out=0;
		ds >> out;
		v<<out;
	}
	return v;
}


// Convenience wrapper to initialize by a QByteArray.
void RNG::init(QByteArray key)
{
	QVector<quint32> init_key=byteArrayToIntVector(key);
	init(init_key);
}

// Convenience wrapper to initialize by a QVector<quint32>.
void RNG::init(QVector<quint32> key)
{
	const size_t sz=key.size();
	//qDebug()<<"VECTOR: "<<key<<", SZ: "<<sz;
	quint32 *init_key=OC_NEW quint32[sz];
	for(size_t i=0; i<sz; ++i) {
		init_key[i]=key[i];
	}
	init(init_key,sz);
	delete[] init_key;
	init_key=nullptr;
}

#include "MersennePRNG.hpp"
#include "EfficientPRNG.hpp"
#include "ParkMillerPRNG.hpp"
#include "DevRandomRNG.hpp"
#include "DevURandomRNG.hpp"
#include "SystemPRNG.hpp"

// Return an instance of a RNG that best matches the given merit
RNG * RNG::sourceFactory(RNGMerit m)
{
	// Create and return a OC_NEW RNG that matches the requested merit
	switch (m) {
	case (EFFICIENT):
		return OC_NEW EfficientPRNG();
	case (SCALABLE):
		return OC_NEW ParkMillerPRNG();
	case (SECURE):
		return OC_NEW DevURandomRNG();
	case (SYSTEM):
		return OC_NEW SystemPRNG();
	case (ACCURATE):
		return OC_NEW DevURandomRNG();
	default:
	case (GENERAL):
		return OC_NEW ParkMillerPRNG();
	}
	return nullptr;
}


// Entropy source factory. Provides new entropy sources based on named implementation
RNG * RNG::sourceFactory(QString name)
{
	// Create and return a new RNG that matches the requested name
	const QString normalized=name.trimmed().toLower();
	if(normalized.startsWith("mers") || normalized.startsWith("mt")) {
		return OC_NEW MersennePRNG();
	}
	if(normalized.startsWith("parkm") || normalized.startsWith("pm")) {
		return OC_NEW ParkMillerPRNG();
	}
	if(normalized.startsWith("devu") || normalized.startsWith("uran")) {
		return OC_NEW DevURandomRNG();
	}
	if(normalized.startsWith("dev") || normalized.startsWith("ran")) {
		return OC_NEW DevRandomRNG();
	}
	if(normalized.startsWith("sys")) {
		return OC_NEW SystemPRNG();
	}
	if(normalized.startsWith("fast") || normalized.startsWith("nois") || normalized.startsWith("effic")) {
		return OC_NEW EfficientPRNG();
	}
	return nullptr;
}
