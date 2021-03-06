#include "qpolarsslpki.hpp"
#include "qpolarsslpki_priv.hpp"
#include "qpolarssltypes.hpp"
///////////////////////////////////////////////////////////////////////////////
namespace qpolarssl
{
///////////////////////////////////////////////////////////////////////////////
Pki::Pki() : d_ptr(new priv::Pki())
{
}

Pki::~Pki()
{
}

bool
Pki::isValid()const
{
	return d_ptr->isValid();
}

void
Pki::reset()
{
	d_ptr->reset();
}

size_t
Pki::keySizeBits()const
{
	return d_ptr->keySizeBits();
}

size_t
Pki::keySizeBytes()const
{
	return d_ptr->keySizeBytes();
}

bool
Pki::canDo(TPki type)const
{
	return d_ptr->canDo( Conversion::toPolar(type) );
}

TPki
Pki::type() const
{
	return Conversion::fromPolar(d_ptr->type());
}

const char*
Pki::name()const
{
	return d_ptr->name();
}

int
Pki::parseKey(const QByteArray& keyData, const QByteArray& password)
{
	return d_ptr->parseKey(QByteArray(keyData), password);
}

int
Pki::parsePublicKey(const QByteArray& keyData)
{
	return d_ptr->parsePublicKey(QByteArray(keyData));
}

int
Pki::parseKeyFrom(const QString& filePath, const QByteArray& password)
{
	return d_ptr->parseKeyFrom(filePath, password);
}

int
Pki::parsePublicKeyFrom(const QString& filePath)
{
	return d_ptr->parsePublicKeyFrom(filePath);
}


int
Pki::generateKeyPair(size_t bits)
{
	return d_ptr->generateRSAKeyPair(bits);
}


QByteArray
Pki::getPEMKey()
{
	return d_ptr->getPEMKey();
}


QByteArray
Pki::getPEMPubkey()
{
	return d_ptr->getPEMPubkey();
}



static QString THashToString(THash algorithm)
{

#define THashToStringStanza(A) case(A): return QStringLiteral(#A);

	switch(algorithm) {
		THashToStringStanza(THash::NONE)           ///< no (invalid) hash
		THashToStringStanza(THash::MD2)            ///< optional
		THashToStringStanza(THash::MD4)            ///< included by default.
		THashToStringStanza(THash::MD5)            ///< included by default.
		THashToStringStanza(THash::SHA1)           ///< included by default.
		THashToStringStanza(THash::SHA224)         ///< included by default.
		THashToStringStanza(THash::SHA256)         ///< included by default.
		THashToStringStanza(THash::SHA384)         ///< included by default.
		THashToStringStanza(THash::SHA512)         ///< included by default.
		THashToStringStanza(THash::RIPEMD160)      ///< optional.
	}

#undef THashToStringStanza
	return "UNKNOWN";
}


QByteArray
Pki::sign(const QByteArray& message, THash algorithm)
{
	const QByteArray ret=d_ptr->sign(message, Conversion::toPolar(algorithm));
	qDebug().noquote().nospace()<<"SIGNING "<<message<<" with "<< THashToString(algorithm)<<" RETURNS '"<<ret<<"'";
	return ret;
}

int
Pki::verify(const QByteArray& message, const QByteArray& signature, THash algorithm)
{
	const int ret= d_ptr->verify(message, signature, Conversion::toPolar(algorithm) );
	qDebug().noquote().nospace()<<"VERIFYING '"<<message<<"' with '"<<signature<<"' and " << THashToString(algorithm) <<" RETURNS "<<QString::number(ret);
	return ret;
}

QByteArray
Pki::encrypt(const QByteArray& hash)
{
	return d_ptr->encrypt(hash);
}

QByteArray
Pki::decrypt(const QByteArray& hash)
{
	return d_ptr->decrypt(hash);
}

///////////////////////////////////////////////////////////////////////////////
} // namespace qpolarssl
///////////////////////////////////////////////////////////////////////////////

QString qPolarDescribeError(int ec)
{
	const size_t 	buflen =1024;
	char buffer[buflen +1];
	mbedtls_strerror(ec,buffer,buflen);
	return QString(buffer);
}
