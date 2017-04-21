#include "RNG.hpp"


//Ctor
RNG::RNG(QString name)
	:name (name)
{

}

RNG::~RNG() {
}

// Fallback implementation that continuously re-seeding using random output combined with key input
inline void RNG::init(quint32 init_key[], size_t key_length) {
	//qDebug()<<"init: ---------"<<key_length;
	//Start with "clean slate"
	init();
	//Follow up with re-seeding using random output combined with key input
	if (key_length > 0){
		for(size_t i=0;i<key_length;++i){
			quint32 k=init_key[i];
			quint32 v=generateInt32();
			quint32 s=(k+1)^v;
			//qDebug()<<"init: "<<k<<v<<s;
			init(s);
		}
	}

}


// Convenience wrapper to initialize by a QByteArray.
void RNG::init(QByteArray key){
	const int l=key.size();
	const int isz=l/4+((l%4)>0?1:0);
	//qDebug()<<"BARRAY: "<<key<<", SZ: "<<l<<isz;
	QVector<quint32> init_key(isz);
	int ii=0;
	union converter{
			quint8 bytes[4];
			quint32 value;
	};
	converter conv;
	conv.value=0;
	for(int i=0;i<4;++i){
		conv.bytes[i]=0;
	}
	for(int i=0;i<l;++i){
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
		const int e1=0;
		const int e2=3;
		const int s=1;
#else
		const int e1=3;
		const int e2=0;
		const int s=-1;
#endif
		int bii=i%4;
		int bi=e1 + bii*s;
		conv.bytes[bi]=key[i];
		//qDebug()<<"init: b="<<QString::number(key[i],16)<<bi<<bii;
		if(e2==bi){
			init_key[ii]=conv.value;
			//qDebug()<<"init:  i="<<conv.value;
			conv.value=0;
			ii++;
		}
	}
	init(init_key);
}

// Convenience wrapper to initialize by a QVector<quint32>.
void RNG::init(QVector<quint32> key){
	const size_t sz=key.size();
	//qDebug()<<"VECTOR: "<<key<<", SZ: "<<sz;
	quint32 *init_key=new quint32[sz];
	for(size_t i=0;i<sz;++i){
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
RNG * RNG::sourceFactory(RNGMerit m) {
	// Create and return a new RNG that matches the requested merit
	switch (m) {
		case (EFFICIENT):
			return new EfficientPRNG();
		case (SCALABLE):
			return new ParkMillerPRNG();
		case (SECURE):
			return new DevRandomRNG();
		case (SYSTEM):
			return new SystemPRNG();
		case (ACCURATE):
			return new DevRandomRNG();
		default:
		case (GENERAL):
			return new ParkMillerPRNG();
	}
	return nullptr;
}


// Entropy source factory. Provides new entropy sources based on named implementation
RNG * RNG::sourceFactory(QString name){
	// Create and return a new RNG that matches the requested name
	const QString normalized=name.trimmed().toLower();
	if(normalized.startsWith("mers") || normalized.startsWith("mt")){
		return new MersennePRNG();
	}
	if(normalized.startsWith("parkm") || normalized.startsWith("pm")){
		return new ParkMillerPRNG();
	}
	if(normalized.startsWith("devu") || normalized.startsWith("uran")){
		return new DevURandomRNG();
	}
	if(normalized.startsWith("dev") || normalized.startsWith("ran")){
		return new DevRandomRNG();
	}
	if(normalized.startsWith("sys")){
		return new SystemPRNG();
	}
	if(normalized.startsWith("fast") || normalized.startsWith("nois") || normalized.startsWith("effic")){
		return new EfficientPRNG();
	}
	return nullptr;
}
