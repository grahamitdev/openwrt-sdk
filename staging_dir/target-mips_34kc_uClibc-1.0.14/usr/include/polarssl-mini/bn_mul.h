/**
 * \file bn_mul.h
 *
 * \brief  Multi-precision integer library
 *
 *  Copyright (C) 2006-2010, Brainspark B.V.
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
/*
 *      Multiply source vector [s] with b, add result
 *       to destination vector [d] and set carry c.
 *
 *      Currently supports:
 *
 *         . IA-32 (386+)         . AMD64 / EM64T
 *         . IA-32 (SSE2)         . Motorola 68000
 *         . PowerPC, 32-bit      . MicroBlaze
 *         . PowerPC, 64-bit      . TriCore
 *         . SPARC v8             . ARM v3+
 *         . Alpha                . MIPS32
 *         . C, longlong          . C, generic
 */
#ifndef POLARSSL_BN_MUL_H
#define POLARSSL_BN_MUL_H

#include "bignum.h"

#define MULADDC_INIT            \
    asm(                        \
        "                       \
        lw     $10, %3;         \
        lw     $11, %4;         \
        lw     $12, %5;         \
        lw     $13, %6;         \
        "

#define MULADDC_CORE            \
        "                       \
        lw     $14, 0($10);     \
        multu  $13, $14;        \
        addi   $10, $10, 4;     \
        mflo   $14;             \
        mfhi   $9;              \
        addu   $14, $12, $14;   \
        lw     $15, 0($11);     \
        sltu   $12, $14, $12;   \
        addu   $15, $14, $15;   \
        sltu   $14, $15, $14;   \
        addu   $12, $12, $9;    \
        sw     $15, 0($11);     \
        addu   $12, $12, $14;   \
        addi   $11, $11, 4;     \
        "

#define MULADDC_STOP            \
        "                       \
        sw     $12, %0;         \
        sw     $11, %1;         \
        sw     $10, %2;         \
        "                       \
        : "=m" (c), "=m" (d), "=m" (s)                      \
        : "m" (s), "m" (d), "m" (c), "m" (b)                \
        : "$9", "$10", "$11", "$12", "$13", "$14", "$15"    \
    );


#endif
