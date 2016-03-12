/** @file qpolarsslpki_priv.hpp
  * Qt wrapper class around polarssl functions
  *
  * @copyright (C) 2015, azadkuh
  * @date 2015.02.12
  * @version 1.0.0
  * @author amir zamani <azadkuh@live.com>
  *
 */

#ifndef QMBEDTLS_PKI_PRIV_HPP
#define QMBEDTLS_PKI_PRIV_HPP

#include <QFile>
#include <QDebug>
#include "mbedtls/pk.h"
#include "mbedtls/error.h"
#include "qpolarsslhash_priv.hpp"
#include "qpolarsslrandom_priv.hpp"
///////////////////////////////////////////////////////////////////////////////
namespace qpolarssl {
	namespace priv {
		///////////////////////////////////////////////////////////////////////////////
		class Pki
		{
			public:
				explicit        Pki(mbedtls_pk_type_t t = MBEDTLS_PK_NONE)
					: Pki(mbedtls_pk_info_from_type(t)) {
				}

				explicit        Pki(const mbedtls_pk_info_t* pinfo) {
					if ( pinfo != nullptr ) {
						mbedtls_pk_setup(context(), pinfo);
						itype   = mbedtls_pk_get_type(context());
					} else {
						mbedtls_pk_init(context());
					}
				}

				virtual        ~Pki() {
					mbedtls_pk_free(context());
				}

				auto            context() -> mbedtls_pk_context* {
					return &ictx;
				}

				auto            context()const -> const mbedtls_pk_context* {
					return &ictx;
				}

				bool            isValid()const {
					return itype != MBEDTLS_PK_NONE;
				}

				void            reset() {
					mbedtls_pk_free(context());
				}

				size_t          keySizeBits()const {
					return mbedtls_pk_get_bitlen(context());
				}

				size_t          keySizeBytes()const {
					return mbedtls_pk_get_len(context());
				}

				bool            canDo(mbedtls_pk_type_t type) {
					return mbedtls_pk_can_do(context(), type) == 1;
				}

				auto            type()const -> mbedtls_pk_type_t {
					return mbedtls_pk_get_type(context());
				}

				auto            name()const -> const char* {
					return mbedtls_pk_get_name(context());
				}

				Random&         random() {
					return irandom;
				}

			public:
				int             parseKey(QByteArray&& keyData,
										 const QByteArray& password = QByteArray()) {
					// new mbedtls pk_parse_key needs null-terminated data
					if ( !keyData.endsWith('\0') )
						keyData.append('\0');

					auto key = reinterpret_cast<const uint8_t*>(keyData.constData());
					auto pwd = reinterpret_cast<const uint8_t*>(
								(password.length() > 0 ) ? password.constData() : nullptr
														   );

					reset();
					int nRet = mbedtls_pk_parse_key(context(),
													key, keyData.length(),
													pwd, password.length()
													);
					if ( nRet != 0 )
						qDebug("pk_parse_key() failed. error: -0x%X", -nRet);

					itype = mbedtls_pk_get_type(context());

					return nRet;
				}

				int             parsePublicKey(QByteArray&& keyData) {
					// new mbedtls pk_parse_key needs null-terminated data
					if ( !keyData.endsWith('\0') )
						keyData.append('\0');

					auto key = reinterpret_cast<const uint8_t*>(keyData.constData());

					reset();
					int nRet = mbedtls_pk_parse_public_key(context(),
														   key, keyData.length()
														   );
					if ( nRet != 0 )
						qDebug("pk_parse_public_key() failed. error: -0x%X", -nRet);

					itype = mbedtls_pk_get_type(context());

					return nRet;
				}

				int             parseKeyFrom(const QString& filePath,
											 const QByteArray& password = QByteArray()) {
					QFile f(filePath);
					QByteArray keyData;
					if ( f.open(QFile::ReadOnly) )
						keyData = f.readAll();

					return parseKey(std::move(keyData), password);
				}

				int             parsePublicKeyFrom(const QString& filePath) {
					QFile f(filePath);
					QByteArray keyData;
					if ( f.open(QFile::ReadOnly) )
						keyData = f.readAll();

					return parsePublicKey(std::move(keyData));
				}



				////////////////////


				int             generateRSAKeyPair(const size_t bits){
					int ret=0;
					itype   = mbedtls_pk_get_type(context());
					qDebug()<<"BEFORE WE START... LETS CHECK SOMETHING: "<<itype;
					reset();
					if((ret=mbedtls_pk_setup(context(), mbedtls_pk_info_from_type(MBEDTLS_PK_RSA)))!=0){
						qWarning("ERROR: could not setup pk -0x%X", -ret);
						//MBEDTLS_PK_RSA
						return ret;
					}
					else{
						qDebug()<<"setup ok";
					}
					itype   = mbedtls_pk_get_type(context());
					if(MBEDTLS_PK_RSA!=itype){
						qWarning()<<"ERROR: Type was not RSA after setup";
						return 1;
					}
					else{
						qDebug()<<"type ok";
					}
					mbedtls_rsa_context *rsa = mbedtls_pk_rsa( ictx);
					//mbedtls_rsa_init( rsa, MBEDTLS_RSA_PKCS_V15, 0 );
					if(0==rsa){
						qWarning()<<"ERROR: rsa = 0";
						return 1;
					}
					mbedtls_entropy_context entropy;
					mbedtls_ctr_drbg_context ctr_drbg;
					const char *pers = "rsa_genkey";
					mbedtls_ctr_drbg_init( &ctr_drbg );
					qDebug()<<"Seeding the random number generator..." ;
					mbedtls_entropy_init( &entropy );
					if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *) pers, strlen( pers ) ) ) != 0 ) {
						qDebug()<<" failed\n  ! mbedtls_ctr_drbg_seed returned" << ret ;
						return ret;
					}
					qDebug()<<" ok\n  . Generating the RSA key of size " <<bits;
					const int exponent=65537;
					if( ( ret = mbedtls_rsa_gen_key( rsa, mbedtls_ctr_drbg_random, &ctr_drbg, bits, exponent ) ) != 0 ){
						qDebug()<<" failed\n  ! mbedtls_rsa_gen_key returned "<<ret;
						return ret;
					}

					//mbedtls_rsa_free( rsa );
					mbedtls_ctr_drbg_free( &ctr_drbg );
					mbedtls_entropy_free( &entropy );

					return 0;
				}
				/////////////////


				QByteArray getPEMKey(){
					size_t pemsize=20000;
					QByteArray ba;
					unsigned char *pembuf=new unsigned char [pemsize];
					memset(pembuf,0x0,pemsize);
					int ret=0;
					if((ret=mbedtls_pk_write_key_pem( &ictx, pembuf, pemsize ))==0){
						ba=QByteArray((char *)pembuf,strlen((char *)pembuf));
					}
					else{
						size_t 	buflen =1024;
						char buffer[buflen +1];
						mbedtls_strerror(ret,buffer,buflen);
						qDebug()<<"ERROR: "<<ret<<"="<<QString(buffer);
					}
					delete [] pembuf;
					return ba;
				}




				QByteArray getPEMPubkey(){
					size_t pemsize=20000;
					QByteArray ba;
					unsigned char *pembuf=new unsigned char [pemsize];
					memset(pembuf,0x0,pemsize);
					int ret=0;
					if((ret=mbedtls_pk_write_pubkey_pem( &ictx, pembuf, pemsize ))==0){
						ba=QByteArray((char *)pembuf,strlen((char *)pembuf));
					}
					else{
						size_t 	buflen =1024;
						char buffer[buflen +1];
						mbedtls_strerror(ret,buffer,buflen);
						qDebug()<<"ERROR: "<<ret<<"="<<QString(buffer);
					}
					delete [] pembuf;
					return ba;
				}



				//////////////////

			public:
				auto            sign(const QByteArray& message,
									 mbedtls_md_type_t algorithm) -> QByteArray {
					auto hash = prepare(message, algorithm);
					uint8_t buffer[MBEDTLS_MPI_MAX_SIZE] = {0};
					size_t  olen = 0;
					int nRet = mbedtls_pk_sign(context(),
											   algorithm,
											   reinterpret_cast<const uint8_t*>(hash.constData()),
											   0,
											   buffer,
											   &olen,
											   mbedtls_ctr_drbg_random,
											   irandom.context()
											   );

					if ( nRet != 0 ) {
						qDebug("pk_sign() failed. error: -0x%X", -nRet);
						return QByteArray();
					}

					return QByteArray((const char*)buffer, olen);
				}

				int             verify(const QByteArray& message,
									   const QByteArray& signature,
									   mbedtls_md_type_t algorithm) {
					auto hash = prepare(message, algorithm);
					return mbedtls_pk_verify(context(),
											 algorithm,
											 reinterpret_cast<const uint8_t*>(hash.constData()),
											 0,
											 reinterpret_cast<const uint8_t*>(signature.constData()),
											 signature.length()
											 );
				}

				auto            encrypt(const QByteArray& hash) -> QByteArray {
					if ( !checkSize(hash) ) {
						qDebug("invalid hash size for encryption!");
						return QByteArray();
					}

					uint8_t buffer[MBEDTLS_MPI_MAX_SIZE] = {0};
					size_t  olen = 0;
					int nRet = mbedtls_pk_encrypt(context(),
												  reinterpret_cast<const uint8_t*>(hash.constData()),
												  hash.length(),
												  buffer,
												  &olen,
												  MBEDTLS_MPI_MAX_SIZE,
												  mbedtls_ctr_drbg_random,
												  irandom.context()
												  );
					if ( nRet != 0 ) {
						qDebug("pk_encrypt() failed. error: -0x%X", -nRet);
						return QByteArray();
					}

					return QByteArray((const char*)buffer, olen);
				}

				auto            decrypt(const QByteArray& hash) -> QByteArray {
					if ( !checkSize(hash) ) {
						qDebug("invalid hash size for decryption!");
						return QByteArray();
					}

					uint8_t buffer[MBEDTLS_MPI_MAX_SIZE] = {0};
					size_t  olen = 0;
					int nRet = mbedtls_pk_decrypt(context(),
												  reinterpret_cast<const uint8_t*>(hash.constData()),
												  hash.length(),
												  buffer,
												  &olen,
												  MBEDTLS_MPI_MAX_SIZE,
												  mbedtls_ctr_drbg_random,
												  irandom.context()
												  );
					if ( nRet != 0 ) {
						qDebug("pk_decrypt() failed. error: -0x%X", -nRet);
						return QByteArray();
					}

					return QByteArray((const char*)buffer, olen);
				}

			protected:
				/// checks if the message needs to be converted to a hash.
				auto            prepare(const QByteArray& message,
										mbedtls_md_type_t algo)const -> QByteArray {
					int   maxLength  = mbedtls_pk_get_len(context());
					return  (message.length() < maxLength   &&  algo == MBEDTLS_MD_NONE)
							? message : Hash::hash(message, algo);
				}

				/// checks if the size of hash is ok for encryption/decryption.
				bool            checkSize(const QByteArray& hash)const {
					int   maxLength  = mbedtls_pk_get_len(context());
					return maxLength >= hash.length();
				}

			protected:
				Q_DISABLE_COPY(Pki)

				Random              irandom;
				mbedtls_pk_type_t   itype = MBEDTLS_PK_NONE;
				mbedtls_pk_context  ictx;
		};
		///////////////////////////////////////////////////////////////////////////////
	} // namespace priv
} // namespace qpolarssl
///////////////////////////////////////////////////////////////////////////////
#endif // QMBEDTLS_PKI_PRIV_HPP
