/**
 * \file asn1.h
 *
 * \brief Generic ASN.1 parsing
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
#ifndef POLARSSL_ASN1_H
#define POLARSSL_ASN1_H

#include "bignum.h"

/**
 * \name ASN1 Error codes
 * These error codes are OR'ed to X509 error codes for
 * higher error granularity. 
 * ASN1 is a standard to specify data structures.
 * \{
 */
#define POLARSSL_ERR_ASN1_OUT_OF_DATA                      -0x0060  /**< Out of data when parsing an ASN1 data structure. */
#define POLARSSL_ERR_ASN1_UNEXPECTED_TAG                   -0x0062  /**< ASN1 tag was of an unexpected value. */
#define POLARSSL_ERR_ASN1_INVALID_LENGTH                   -0x0064  /**< Error when trying to determine the length or invalid length. */
#define POLARSSL_ERR_ASN1_LENGTH_MISMATCH                  -0x0066  /**< Actual length differs from expected length. */
#define POLARSSL_ERR_ASN1_INVALID_DATA                     -0x0068  /**< Data is invalid. (not used) */
#define POLARSSL_ERR_ASN1_MALLOC_FAILED                    -0x006A  /**< Memory allocation failed */
#define POLARSSL_ERR_ASN1_BUF_TOO_SMALL                    -0x006C  /**< Buffer too small when writing ASN.1 data structure. */

/**
 * \name DER constants
 * These constants comply with DER encoded the ANS1 type tags.
 * DER encoding uses hexadecimal representation.
 * An example DER sequence is:\n
 * - 0x02 -- tag indicating INTEGER
 * - 0x01 -- length in octets
 * - 0x05 -- value
 * Such sequences are typically read into \c ::x509_buf.
 * \{
 */
#define ASN1_BOOLEAN                 0x01
#define ASN1_INTEGER                 0x02
#define ASN1_BIT_STRING              0x03
#define ASN1_OCTET_STRING            0x04
#define ASN1_NULL                    0x05
#define ASN1_OID                     0x06
#define ASN1_UTF8_STRING             0x0C
#define ASN1_SEQUENCE                0x10
#define ASN1_SET                     0x11
#define ASN1_PRINTABLE_STRING        0x13
#define ASN1_T61_STRING              0x14
#define ASN1_IA5_STRING              0x16
#define ASN1_UTC_TIME                0x17
#define ASN1_GENERALIZED_TIME        0x18
#define ASN1_UNIVERSAL_STRING        0x1C
#define ASN1_BMP_STRING              0x1E
#define ASN1_PRIMITIVE               0x00
#define ASN1_CONSTRUCTED             0x20
#define ASN1_CONTEXT_SPECIFIC        0x80

/** Returns the size of the binary string, without the trailing \\0 */
#define OID_SIZE(x) (sizeof(x) - 1)

/** Compares two asn1_buf structures for the same OID. Only works for
 *  'defined' oid_str values (OID_HMAC_SHA1), you cannot use a 'unsigned
 *  char *oid' here!
 */
#define OID_CMP(oid_str, oid_buf)                                   \
        ( ( OID_SIZE(oid_str) == (oid_buf)->len ) &&                \
          memcmp( (oid_str), (oid_buf)->p, (oid_buf)->len) == 0 )


/**
 * \name asn1 error codes
 * these error codes are or'ed to x509 error codes for
 * higher error granularity. 
 * asn1 is a standard to specify data structures.
 * \{
 */
#define polarssl_err_asn1_out_of_data                      -0x0060  /**< out of data when parsing an asn1 data structure. */
#define polarssl_err_asn1_unexpected_tag                   -0x0062  /**< asn1 tag was of an unexpected value. */
#define polarssl_err_asn1_invalid_length                   -0x0064  /**< error when trying to determine the length or invalid length. */
#define polarssl_err_asn1_length_mismatch                  -0x0066  /**< actual length differs from expected length. */
#define polarssl_err_asn1_invalid_data                     -0x0068  /**< data is invalid. (not used) */
#define polarssl_err_asn1_malloc_failed                    -0x006a  /**< memory allocation failed */
#define polarssl_err_asn1_buf_too_small                    -0x006c  /**< buffer too small when writing asn.1 data structure. */

/**
 * Type-length-value structure that allows for ASN1 using DER.
 */
typedef struct _asn1_buf
{
    int tag;                /**< ASN1 type, e.g. ASN1_UTF8_STRING. */
    size_t len;             /**< ASN1 length, e.g. in octets. */
    unsigned char *p;       /**< ASN1 data, e.g. in ASCII. */
}
asn1_buf;

/**
 * Container for ASN1 bit strings.
 */
typedef struct _asn1_bitstring
{
    size_t len;                 /**< ASN1 length, e.g. in octets. */
    unsigned char unused_bits;  /**< Number of unused bits at the end of the string */
    unsigned char *p;           /**< Raw ASN1 data for the bit string */
}
asn1_bitstring;

/**
 * Container for a sequence of ASN.1 items
 */
typedef struct _asn1_sequence
{
    asn1_buf buf;                   /**< Buffer containing the given ASN.1 item. */
    struct _asn1_sequence *next;    /**< The next entry in the sequence. */
}
asn1_sequence;

/**
 * Container for a sequence or list of 'named' ASN.1 data items
 */
typedef struct _asn1_named_data
{
    asn1_buf oid;                   /**< The object identifier. */
    asn1_buf val;                   /**< The named value. */
    struct _asn1_named_data *next;  /**< The next entry in the sequence. */
}
asn1_named_data;

/**
 * \brief       Get the length of an ASN.1 element.
 *              Updates the pointer to immediately behind the length.
 *
 * \param p     The position in the ASN.1 data
 * \param end   End of data
 * \param len   The variable that will receive the value
 *
 * \return      0 if successful, POLARSSL_ERR_ASN1_OUT_OF_DATA on reaching
 *              end of data, POLARSSL_ERR_ASN1_INVALID_LENGTH if length is
 *              unparseable.
 */
int asn1_get_len( unsigned char **p,
                  const unsigned char *end,
                  size_t *len );

/**
 * \brief       Get the tag and length of the tag. Check for the requested tag.
 *              Updates the pointer to immediately behind the tag and length.
 *
 * \param p     The position in the ASN.1 data
 * \param end   End of data
 * \param len   The variable that will receive the length
 * \param tag   The expected tag
 *
 * \return      0 if successful, POLARSSL_ERR_ASN1_UNEXPECTED_TAG if tag did
 *              not match requested tag, or another specific ASN.1 error code.
 */
int asn1_get_tag( unsigned char **p,
                  const unsigned char *end,
                  size_t *len, int tag );


#endif
