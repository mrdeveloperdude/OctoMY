
II=$$TOP_PWD/libs/libqpolarssl/mbedtls/include
I=$$II/mbedtls
L=$$TOP_PWD/libs/libqpolarssl/mbedtls/library

INCLUDEPATH += $$II/


SOURCES += \
	$$L/aes.c \
	$$L/aesni.c \
	$$L/asn1parse.c \
	$$L/asn1write.c \
	$$L/base64.c \
	$$L/bignum.c \
	$$L/blowfish.c \
	$$L/cipher.c \
	$$L/cipher_wrap.c \
	$$L/ctr_drbg.c \
	$$L/des.c \
	$$L/entropy.c \
	$$L/entropy_poll.c \
	$$L/error.c \
	$$L/md4.c \
	$$L/md5.c \
	$$L/md.c \
	$$L/md_wrap.c \
	$$L/oid.c \
	$$L/pem.c \
	$$L/pk.c \
	$$L/pkparse.c \
	$$L/pk_wrap.c \
	$$L/pkwrite.c \
	$$L/platform.c \
	$$L/rsa.c \
	$$L/sha1.c \
	$$L/sha256.c \
	$$L/sha512.c \



HEADERS  += \
	$$I/aes.h \
	$$I/aesni.h \
	$$I/arc4.h \
	$$I/asn1.h \
	$$I/asn1write.h \
	$$I/base64.h \
	$$I/bignum.h \
	$$I/blowfish.h \
	$$I/bn_mul.h \
	$$I/camellia.h \
	$$I/ccm.h \
	$$I/certs.h \
	$$I/check_config.h \
	$$I/cipher.h \
	$$I/cipher_internal.h \
	$$I/compat-1.3.h \
	$$I/config.h \
	$$I/ctr_drbg.h \
	$$I/debug.h \
	$$I/des.h \
	$$I/dhm.h \
	$$I/ecdh.h \
	$$I/ecdsa.h \
	$$I/ecjpake.h \
	$$I/ecp.h \
	$$I/entropy.h \
	$$I/entropy_poll.h \
	$$I/error.h \
	$$I/gcm.h \
	$$I/havege.h \
	$$I/hmac_drbg.h \
	$$I/md2.h \
	$$I/md4.h \
	$$I/md5.h \
	$$I/md.h \
	$$I/md_internal.h \
	$$I/memory_buffer_alloc.h \
	$$I/net.h \
	$$I/oid.h \
	$$I/padlock.h \
	$$I/pem.h \
	$$I/pkcs11.h \
	$$I/pkcs12.h \
	$$I/pkcs5.h \
	$$I/pk.h \
	$$I/pk_internal.h \
	$$I/platform.h \
	$$I/ripemd160.h \
	$$I/rsa.h \
	$$I/sha1.h \
	$$I/sha256.h \
	$$I/sha512.h \
	$$I/ssl_cache.h \
	$$I/ssl_ciphersuites.h \
	$$I/ssl_cookie.h \
	$$I/ssl.h \
	$$I/ssl_internal.h \
	$$I/ssl_ticket.h \
	$$I/threading.h \
	$$I/timing.h \
	$$I/version.h \
	$$I/x509_crl.h \
	$$I/x509_crt.h \
	$$I/x509_csr.h \
	$$I/x509.h \
	$$I/xtea.h \


