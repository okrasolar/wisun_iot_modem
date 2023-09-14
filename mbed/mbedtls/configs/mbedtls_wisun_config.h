/*
 * Copyright (c) 2019, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MBEDTLS_WISUN_CONFIG_H_
#define MBEDTLS_WISUN_CONFIG_H_

#define MBEDTLS_ENTROPY_MAX_SOURCES 4      // save 320 bytes of RAM (default is 20 entropy source)
#define MBEDTLS_SSL_MAX_CONTENT_LEN 4096   // Use 4kB buffers for input and output instead of 16kB

// #define MBEDTLS_DEBUG_C
#undef MBEDTLS_DEBUG_C
#undef MBEDTLS_SSL_SRV_C

/* mbed TLS feature support */
#define MBEDTLS_ECP_NIST_OPTIM
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#undef MBEDTLS_ECP_DP_SECP192R1_ENABLED
#undef MBEDTLS_ECP_DP_SECP224R1_ENABLED
#undef MBEDTLS_ECP_DP_SECP384R1_ENABLED
#undef MBEDTLS_ECP_DP_SECP521R1_ENABLED
#undef MBEDTLS_ECP_DP_SECP192K1_ENABLED
#undef MBEDTLS_ECP_DP_SECP224K1_ENABLED
#undef MBEDTLS_ECP_DP_SECP256K1_ENABLED
#undef MBEDTLS_ECP_DP_BP256R1_ENABLED
#undef MBEDTLS_ECP_DP_BP384R1_ENABLED
#undef MBEDTLS_ECP_DP_BP512R1_ENABLED
#undef MBEDTLS_ECP_DP_CURVE25519_ENABLED

#ifdef MBEDTLS_SSL_TLS_C
// KV Code coming here
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_SSL_EXPORT_KEYS
#define MBEDTLS_SSL_ALL_ALERT_MESSAGES
#endif

/* mbed TLS modules */
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_AES_C
#define MBEDTLS_CCM_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_ECP_C
#define MBEDTLS_MD_C
#define MBEDTLS_PK_C
#define MBEDTLS_SHA256_C
#ifdef MBEDTLS_SSL_TLS_C
#define MBEDTLS_SSL_COOKIE_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_SRV_C
#endif
#define MBEDTLS_NIST_KW_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_SHA1_C

/* Save RAM at the expense of ROM */
#define MBEDTLS_AES_ROM_TABLES
#define MBEDTLS_AES_FEWER_TABLES

/* Save RAM by adjusting to our exact needs */
#define MBEDTLS_ECP_MAX_BITS 256
#undef MBEDTLS_MPI_MAX_SIZE
#define MBEDTLS_MPI_MAX_SIZE 128   // 256 bits is 32 bytes

/* Save ROM and a few bytes of RAM by specifying our own ciphersuite list */
#define MBEDTLS_SSL_CIPHERSUITES MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8

/* Optimization. Remove all not needed stuff */
#define MBEDTLS_X509_USE_C
#define MBEDTLS_X509_CRT_PARSE_C
#undef MBEDTLS_SSL_SERVER_NAME_INDICATION
#undef MBEDTLS_SELF_TEST
#undef MBEDTLS_SSL_ENCRYPT_THEN_MAC
#undef MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_ECDH_C
#define MBEDTLS_ECDSA_C
#undef MBEDTLS_ERROR_C
#define MBEDTLS_GCM_C
#define MBEDTLS_OID_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_PK_WRITE_C
#undef MBEDTLS_RSA_C
#undef MBEDTLS_VERSION_C
#undef MBEDTLS_CERTS_C
#undef MBEDTLS_HMAC_DRBG_C
#define MBEDTLS_CIPHER_MODE_CBC
#undef MBEDTLS_CIPHER_PADDING_PKCS7
#undef MBEDTLS_ECDSA_DETERMINISTIC
#undef MBEDTLS_SSL_SESSION_TICKETS
#undef MBEDTLS_VERSION_FEATURES
#undef MBEDTLS_X509_CHECK_KEY_USAGE
#undef MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE
#undef MBEDTLS_ERROR_STRERROR_DUMMY
#undef MBEDTLS_SSL_ALPN
#undef MBEDTLS_SSL_TICKET_C
#undef MBEDTLS_HAVE_SSE2
// #undef MBEDTLS_PLATFORM_MEMORY
#undef MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
#undef MBEDTLS_DEPRECATED_WARNING
#undef MBEDTLS_DEPRECATED_REMOVED
#undef MBEDTLS_CAMELLIA_SMALL_MEMORY
#undef MBEDTLS_CIPHER_MODE_CFB
#undef MBEDTLS_CIPHER_MODE_CTR
#undef MBEDTLS_CIPHER_NULL_CIPHER
#undef MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS
#undef MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN
#undef MBEDTLS_CIPHER_PADDING_ZEROS
#undef MBEDTLS_ENABLE_WEAK_CIPHERSUITES
#define MBEDTLS_REMOVE_ARC4_CIPHERSUITES
#undef MBEDTLS_ECP_DP_CURVE25519_ENABLED
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED
#undef MBEDTLS_PK_PARSE_EC_EXTENDED
#undef MBEDTLS_GENPRIME
#undef MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
#undef MBEDTLS_MEMORY_DEBUG
#undef MBEDTLS_MEMORY_BACKTRACE
#undef MBEDTLS_PK_RSA_ALT_SUPPORT
#undef MBEDTLS_PKCS1_V15
#undef MBEDTLS_PKCS1_V21
#undef MBEDTLS_RSA_NO_CRT
#undef MBEDTLS_SSL_AEAD_RANDOM_IV
#undef MBEDTLS_RSA_NO_CRT
#undef MBEDTLS_SSL_DEBUG_ALL
#undef MBEDTLS_SSL_FALLBACK_SCSV
#undef MBEDTLS_SSL_HW_RECORD_ACCEL
#undef MBEDTLS_SSL_CBC_RECORD_SPLITTING
#undef MBEDTLS_SSL_RENEGOTIATION
#undef MBEDTLS_SSL_SRV_SUPPORT_SSLV2_CLIENT_HELLO
#undef MBEDTLS_SSL_SRV_RESPECT_CLIENT_PREFERENCE
#undef MBEDTLS_SSL_PROTO_TLS1
#undef MBEDTLS_SSL_PROTO_TLS1_1
#undef MBEDTLS_SSL_DTLS_BADMAC_LIMIT
#undef MBEDTLS_SSL_TRUNCATED_HMAC
#undef MBEDTLS_THREADING_ALT
#undef MBEDTLS_THREADING_PTHREAD
#undef MBEDTLS_X509_ALLOW_EXTENSIONS_NON_V3
#undef MBEDTLS_X509_ALLOW_UNSUPPORTED_CRITICAL_EXTENSION
#undef MBEDTLS_X509_RSASSA_PSS_SUPPORT
#undef MBEDTLS_ZLIB_SUPPORT
#undef MBEDTLS_AESNI_C
#undef MBEDTLS_ARC4_C
#undef MBEDTLS_BLOWFISH_C
#undef MBEDTLS_CAMELLIA_C
#undef MBEDTLS_DES_C
#undef MBEDTLS_DHM_C
#undef MBEDTLS_HAVEGE_C
#undef MBEDTLS_MD2_C
#undef MBEDTLS_MD4_C
#undef MBEDTLS_MD5_C
#undef MBEDTLS_MEMORY_BUFFER_ALLOC_C
#undef MBEDTLS_PADLOCK_C
#undef MBEDTLS_PEM_WRITE_C
#undef MBEDTLS_PKCS5_C
#undef MBEDTLS_PKCS11_C
#undef MBEDTLS_PKCS12_C
// #undef MBEDTLS_PLATFORM_C
#undef MBEDTLS_RIPEMD160_C
#undef MBEDTLS_SHA512_C
#undef MBEDTLS_SSL_CACHE_C
#undef MBEDTLS_THREADING_C
#define MBEDTLS_X509_CREATE_C
#define MBEDTLS_X509_CRT_WRITE_C
#undef MBEDTLS_XTEA_C

// TI Platform Specific Configurations
#undef MBEDTLS_SSL_PROTO_DTLS
#undef MBEDTLS_SSL_DTLS_HELLO_VERIFY
#undef MBEDTLS_SSL_DTLS_ANTI_REPLAY
#undef MBEDTLS_SSL_DTLS_CONNECTION_ID
#undef MBEDTLS_SSL_DTLS_BADMAC_LIMIT
#undef MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#define MBEDTLS_AES_ALT
// #define MBEDTLS_SHA256_ALT
#include "mbedtls/check_config.h"

#endif /* MBEDTLS_WISUN_CONFIG_H */
