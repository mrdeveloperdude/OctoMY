TARGET =	pki
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)

INCLUDEPATH += .
INCLUDEPATH += mbedtls/include/

DEFINES     += MBEDTLS_CONFIG_FILE='\\"mbedtls_config.h\\"'
HEADERS     += mbedtls_config.h

SOURCES     += \
	mbedtls/library/base64.c \
	mbedtls/library/md4.c \
	mbedtls/library/md5.c \
	mbedtls/library/sha1.c \
	mbedtls/library/sha256.c \
	mbedtls/library/sha512.c \
	mbedtls/library/md_wrap.c \
	mbedtls/library/md.c \
	mbedtls/library/blowfish.c \
	mbedtls/library/aes.c \
	mbedtls/library/aesni.c \
	mbedtls/library/des.c \
	mbedtls/library/cipher_wrap.c \
	mbedtls/library/cipher.c \
	mbedtls/library/entropy.c \
	mbedtls/library/entropy_poll.c \
	mbedtls/library/ctr_drbg.c \
	mbedtls/library/rsa.c \
	mbedtls/library/pem.c \
	mbedtls/library/bignum.c \
	mbedtls/library/oid.c \
	mbedtls/library/asn1parse.c \
	mbedtls/library/pkparse.c \
	mbedtls/library/pk_wrap.c \
	mbedtls/library/pk.c \
	mbedtls/library/oid.c \
	mbedtls/library/platform.c \
	mbedtls/library/pkwrite.c \
	mbedtls/library/asn1write.c \
	mbedtls/library/error.c

SOURCES  += \
	qpolarsslhash.cpp \
	qpolarsslcipher.cpp \
	qpolarsslrandom.cpp \
	qpolarsslpki.cpp

HEADERS  += \
	qpolarssltypes.hpp \
	qpolarsslcipher_priv.hpp \
	qpolarsslhash_priv.hpp \
	qpolarsslpki_priv.hpp \
	qpolarsslrandom_priv.hpp \
	qpolarsslbase.hpp \
	qpolarsslhash.hpp \
	qpolarsslcipher.hpp \
	qpolarsslrandom.hpp \
	qpolarsslpki.hpp


win32:LIBS += -ladvapi32
