/**
 * \file oid.c
 *
 * \brief Object Identifier (OID) database
 *
 *  Copyright (C) 2006-2013, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include "oid.h"
#include "rsa.h"

/*
 * Macro to automatically add the size of #define'd OIDs
 */
#define ADD_LEN(s)      s, OID_SIZE(s)

/*
 * Macro to generate an internal function for oid_XXX_from_asn1() (used by
 * the other functions)
 */
#define FN_OID_TYPED_FROM_ASN1( TYPE_T, NAME, LIST )                        \
static const TYPE_T * oid_ ## NAME ## _from_asn1( const asn1_buf *oid )     \
{                                                                           \
    const TYPE_T *p = LIST;                                                 \
    const oid_descriptor_t *cur = (const oid_descriptor_t *) p;             \
    if( p == NULL || oid == NULL ) return( NULL );                          \
    while( cur->asn1 != NULL ) {                                            \
        if( cur->asn1_len == oid->len &&                                    \
            memcmp( cur->asn1, oid->p, oid->len ) == 0 ) {                  \
            return( p );                                                    \
        }                                                                   \
        p++;                                                                \
        cur = (const oid_descriptor_t *) p;                                 \
    }                                                                       \
    return( NULL );                                                         \
}

/*
 * Macro to generate a function for retrieving a single attribute from the
 * descriptor of an oid_descriptor_t wrapper.
 */
#define FN_OID_GET_DESCRIPTOR_ATTR1(FN_NAME, TYPE_T, TYPE_NAME, ATTR1_TYPE, ATTR1) \
int FN_NAME( const asn1_buf *oid, ATTR1_TYPE * ATTR1 )                  \
{                                                                       \
    const TYPE_T *data = oid_ ## TYPE_NAME ## _from_asn1( oid );        \
    if( data == NULL ) return ( POLARSSL_ERR_OID_NOT_FOUND );           \
    *ATTR1 = data->descriptor.ATTR1;                                    \
    return( 0 );                                                        \
}

/*
 * Macro to generate a function for retrieving the OID based on a single
 * attribute from a oid_descriptor_t wrapper.
 */
#define FN_OID_GET_OID_BY_ATTR1(FN_NAME, TYPE_T, LIST, ATTR1_TYPE, ATTR1)   \
int FN_NAME( ATTR1_TYPE ATTR1, const char **oid, size_t *olen )             \
{                                                                           \
    const TYPE_T *cur = LIST;                                               \
    while( cur->descriptor.asn1 != NULL ) {                                 \
        if( cur->ATTR1 == ATTR1 ) {                                         \
            *oid = cur->descriptor.asn1;                                    \
            *olen = cur->descriptor.asn1_len;                               \
            return( 0 );                                                    \
        }                                                                   \
        cur++;                                                              \
    }                                                                       \
    return( POLARSSL_ERR_OID_NOT_FOUND );                                   \
}

/*
 * Macro to generate a function for retrieving a single attribute from an
 * oid_descriptor_t wrapper.
 */
#define FN_OID_GET_ATTR1(FN_NAME, TYPE_T, TYPE_NAME, ATTR1_TYPE, ATTR1) \
int FN_NAME( const asn1_buf *oid, ATTR1_TYPE * ATTR1 )                  \
{                                                                       \
    const TYPE_T *data = oid_ ## TYPE_NAME ## _from_asn1( oid );        \
    if( data == NULL ) return ( POLARSSL_ERR_OID_NOT_FOUND );           \
    *ATTR1 = data->ATTR1;                                               \
    return( 0 );                                                        \
}

/*
 * For digestAlgorithm
 */
typedef struct {
    oid_descriptor_t    descriptor;
    md_type_t           md_alg;
} oid_md_alg_t;

static const oid_md_alg_t oid_md_alg[] =
{
    {
        { ADD_LEN( OID_DIGEST_ALG_MD2 ),       "id-md2",       "MD2" },
        POLARSSL_MD_MD2,
    },
    {
        { ADD_LEN( OID_DIGEST_ALG_MD4 ),       "id-md4",       "MD4" },
        POLARSSL_MD_MD4,
    },
    {
        { ADD_LEN( OID_DIGEST_ALG_MD5 ),       "id-md5",       "MD5" },
        POLARSSL_MD_MD5,
    },
    {
        { ADD_LEN( OID_DIGEST_ALG_SHA1 ),      "id-sha1",      "SHA-1" },
        POLARSSL_MD_SHA1,
    },
    {
        { ADD_LEN( OID_DIGEST_ALG_SHA1 ),      "id-sha1",      "SHA-1" },
        POLARSSL_MD_SHA1,
    },
    {
        { ADD_LEN( OID_DIGEST_ALG_SHA224 ),    "id-sha224",    "SHA-224" },
        POLARSSL_MD_SHA224,
    },
    {
        { ADD_LEN( OID_DIGEST_ALG_SHA256 ),    "id-sha256",    "SHA-256" },
        POLARSSL_MD_SHA256,
    },
    {
        { ADD_LEN( OID_DIGEST_ALG_SHA384 ),    "id-sha384",    "SHA-384" },
        POLARSSL_MD_SHA384,
    },
    {
        { ADD_LEN( OID_DIGEST_ALG_SHA512 ),    "id-sha512",    "SHA-512" },
        POLARSSL_MD_SHA512,
    },
    {
        { NULL, 0, NULL, NULL },
        0,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_md_alg_t, md_alg, oid_md_alg);
FN_OID_GET_ATTR1(oid_get_md_alg, oid_md_alg_t, md_alg, md_type_t, md_alg);

