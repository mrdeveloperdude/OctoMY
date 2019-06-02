/**
 * \file config.h
 *
 * \brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

// NOTE: THIS IS THE CONFIGURATION AS ADAPTED FOR OCTOMY

// base
#define MBEDTLS_BASE64_C

// hash
#define MBEDTLS_MD4_C
#define MBEDTLS_MD5_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA512_C
#define MBEDTLS_MD_C

// cipher
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_CIPHER_MODE_WITH_PADDING
#define MBEDTLS_CIPHER_PADDING_PKCS7
#define MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS
#define MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN
#define MBEDTLS_CIPHER_PADDING_ZEROS

#define MBEDTLS_HAVE_ASM
#define MBEDTLS_CIPHER_C
#define MBEDTLS_DES_C
#define MBEDTLS_PADLOCK_C
#define MBEDTLS_BLOWFISH_C
#define MBEDTLS_AES_C
#define MBEDTLS_AESNI_C

// random number generator and entropy
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_ENTROPY_C

// PKI cryptography
#define MBEDTLS_PK_C
#define MBEDTLS_RSA_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_OID_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_PKCS1_V21

#define MBEDTLS_PLATFORM_C
#define MBEDTLS_FS_IO
#define MBEDTLS_OID_C
#define MBEDTLS_PLATFORM_SNPRINTF_ALT
#define MBEDTLS_GENPRIME
#define MBEDTLS_PEM_WRITE_C
#define MBEDTLS_PK_WRITE_C
#define MBEDTLS_ASN1_WRITE_C

#define MBEDTLS_ERROR_C

#include "check_config.h"


#endif /* MBEDTLS_CONFIG_H */
