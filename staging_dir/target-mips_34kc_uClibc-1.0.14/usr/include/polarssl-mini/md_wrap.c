/**
 * \file md_wrap.c

 * \brief Generic message digest wrapper for PolarSSL
 *
 * \author Adriaan de Jong <dejong@fox-it.com>
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

#include "md_wrap.h"
#include "sha256.h"


static void sha256_starts_wrap( void *ctx )
{
    sha256_starts( (sha256_context *) ctx );
}

static void sha256_update_wrap( void *ctx, const unsigned char *input, size_t ilen )
{
    sha256_update( (sha256_context *) ctx, input, ilen );
}

static void sha256_finish_wrap( void *ctx, unsigned char *output )
{
    sha256_finish( (sha256_context *) ctx, output );
}

static void sha256_wrap( const unsigned char *input, size_t ilen,
                    unsigned char *output )
{
    sha256( input, ilen, output, 0 );
}

static int sha256_file_wrap( const char *path, unsigned char *output )
{
#if defined(POLARSSL_FS_IO)
    return sha256_file( path, output, 0 );
#else
    ((void) path);
    ((void) output);
    return POLARSSL_ERR_MD_FEATURE_UNAVAILABLE;
#endif
}

static void sha256_hmac_starts_wrap( void *ctx, const unsigned char *key, size_t keylen )
{
    sha256_hmac_starts( (sha256_context *) ctx, key, keylen, 0 );
}

static void sha256_hmac_update_wrap( void *ctx, const unsigned char *input, size_t ilen )
{
    sha256_hmac_update( (sha256_context *) ctx, input, ilen );
}

static void sha256_hmac_finish_wrap( void *ctx, unsigned char *output )
{
    sha256_hmac_finish( (sha256_context *) ctx, output );
}

static void sha256_hmac_reset_wrap( void *ctx )
{
    sha256_hmac_reset( (sha256_context *) ctx );
}

static void sha256_hmac_wrap( const unsigned char *key, size_t keylen,
        const unsigned char *input, size_t ilen,
        unsigned char *output )
{
    sha256_hmac( key, keylen, input, ilen, output, 0 );
}

static void * sha256_ctx_alloc( void )
{
    return malloc( sizeof( sha256_context ) );
}

static void sha256_ctx_free( void *ctx )
{
    free( ctx );
}

static void sha256_process_wrap( void *ctx, const unsigned char *data )
{
    sha256_process( (sha256_context *) ctx, data );
}

const md_info_t sha256_info = {
    POLARSSL_MD_SHA256,
    "SHA256",
    32,
    sha256_starts_wrap,
    sha256_update_wrap,
    sha256_finish_wrap,
    sha256_wrap,
    sha256_file_wrap,
    sha256_hmac_starts_wrap,
    sha256_hmac_update_wrap,
    sha256_hmac_finish_wrap,
    sha256_hmac_reset_wrap,
    sha256_hmac_wrap,
    sha256_ctx_alloc,
    sha256_ctx_free,
    sha256_process_wrap,
};

