/* TCC runtime library. 
   Parts of this code are (c) 2002 Fabrice Bellard 

   Copyright (C) 1987, 1988, 1992, 1994, 1995 Free Software Foundation, Inc.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

In addition to the permissions in the GNU General Public License, the
Free Software Foundation gives you unlimited permission to link the
compiled version of this file into combinations with other programs,
and to distribute those combinations without any restriction coming
from the use of this file.  (The General Public License restrictions
do apply in other respects; for example, they cover modification of
the file, and distribution when not linked into a combine
executable.)

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  
*/

#define W_TYPE_SIZE   32
#define BITS_PER_UNIT 8

typedef int Wtype;
typedef unsigned int UWtype;
typedef unsigned int USItype;
typedef long long DWtype;
typedef unsigned long long UDWtype;

struct DWstruct {
    Wtype low, high;
};

typedef union
{
  struct DWstruct s;
  DWtype ll;
} DWunion;

typedef long double XFtype;
#define WORD_SIZE (sizeof (Wtype) * BITS_PER_UNIT)
#define HIGH_WORD_COEFF (((UDWtype) 1) << WORD_SIZE)

/* the following deal with IEEE single-precision numbers */
#define EXCESS		126
#define SIGNBIT		0x80000000
#define HIDDEN		(1 << 23)
#define SIGN(fp)	((fp) & SIGNBIT)
#define EXP(fp)		(((fp) >> 23) & 0xFF)
#define MANT(fp)	(((fp) & 0x7FFFFF) | HIDDEN)
#define PACK(s,e,m)	((s) | ((e) << 23) | (m))

/* the following deal with IEEE double-precision numbers */
#define EXCESSD		1022
#define HIDDEND		(1 << 20)
#define EXPD(fp)	(((fp.l.upper) >> 20) & 0x7FF)
#define SIGND(fp)	((fp.l.upper) & SIGNBIT)
#define MANTD(fp)	(((((fp.l.upper) & 0xFFFFF) | HIDDEND) << 10) | \
				(fp.l.lower >> 22))
#define HIDDEND_LL	((long long)1 << 52)
#define MANTD_LL(fp)	((fp.ll & (HIDDEND_LL-1)) | HIDDEND_LL)
#define PACKD_LL(s,e,m)	(((long long)((s)+((e)<<20))<<32)|(m))

/* the following deal with x86 long double-precision numbers */
#define EXCESSLD	16382
#define EXPLD(fp)	(fp.l.upper & 0x7fff)
#define SIGNLD(fp)	((fp.l.upper) & 0x8000)

/* only for x86 */
union ldouble_long {
    long double ld;
    struct {
        unsigned long long lower;
        unsigned short upper;
    } l;
};

union double_long {
    double d;
#if 1
    struct {
        unsigned int lower;
        int upper;
    } l;
#else
    struct {
        int upper;
        unsigned int lower;
    } l;
#endif
    long long ll;
};

union float_long {
    float f;
    unsigned int l;
};

/* XXX: fix tcc's code generator to do this instead */
float __floatundisf(unsigned long long a)
{
    DWunion uu; 
    XFtype r;

    uu.ll = a;
    if (uu.s.high >= 0) {
        return (float)uu.ll;
    } else {
        r = (XFtype)uu.ll;
        r += 18446744073709551616.0;
        return (float)r;
    }
}

double __floatundidf(unsigned long long a)
{
    DWunion uu; 
    XFtype r;

    uu.ll = a;
    if (uu.s.high >= 0) {
        return (double)uu.ll;
    } else {
        r = (XFtype)uu.ll;
        r += 18446744073709551616.0;
        return (double)r;
    }
}

long double __floatundixf(unsigned long long a)
{
    DWunion uu; 
    XFtype r;

    uu.ll = a;
    if (uu.s.high >= 0) {
        return (long double)uu.ll;
    } else {
        r = (XFtype)uu.ll;
        r += 18446744073709551616.0;
        return (long double)r;
    }
}

unsigned long long __fixunssfdi (float a1)
{
    register union float_long fl1;
    register int exp;
    register unsigned long long l;

    fl1.f = a1;

    if (fl1.l == 0)
	return (0);

    exp = EXP (fl1.l) - EXCESS - 24;
    l = MANT(fl1.l);

    if (exp >= 41)
        return 1ULL << 63;
    else if (exp >= 0)
        l <<= exp;
    else if (exp >= -23)
        l >>= -exp;
    else
	return 0;
    if (SIGN(fl1.l))
        l = (unsigned long long)-l;
    return l;
}

long long __fixsfdi (float a1)
{
    long long ret; int s;
    ret = __fixunssfdi((s = a1 >= 0) ? a1 : -a1);
    return s ? ret : -ret;
}

unsigned long long __fixunsdfdi (double a1)
{
    register union double_long dl1;
    register int exp;
    register unsigned long long l;

    dl1.d = a1;

    if (dl1.ll == 0)
	return (0);

    exp = EXPD (dl1) - EXCESSD - 53;
    l = MANTD_LL(dl1);

    if (exp >= 12)
        return 1ULL << 63; /* overflow result (like gcc, somewhat) */
    else if (exp >= 0)
        l <<= exp;
    else if (exp >= -52)
        l >>= -exp;
    else
        return 0;
    if (SIGND(dl1))
        l = (unsigned long long)-l;
    return l;
}

long long __fixdfdi (double a1)
{
    long long ret; int s;
    ret = __fixunsdfdi((s = a1 >= 0) ? a1 : -a1);
    return s ? ret : -ret;
}

#ifndef __arm__
unsigned long long __fixunsxfdi (long double a1)
{
    register union ldouble_long dl1;
    register int exp;
    register unsigned long long l;

    dl1.ld = a1;

    if (dl1.l.lower == 0 && dl1.l.upper == 0)
	return (0);

    exp = EXPLD (dl1) - EXCESSLD - 64;
    l = dl1.l.lower;
    if (exp > 0)
	return 1ULL << 63;
    if (exp < -63)
        return 0;
    l >>= -exp;
    if (SIGNLD(dl1))
        l = (unsigned long long)-l;
    return l;
}

long long __fixxfdi (long double a1)
{
    long long ret; int s;
    ret = __fixunsxfdi((s = a1 >= 0) ? a1 : -a1);
    return s ? ret : -ret;
}
#endif /* !ARM */

#if defined _WIN64
/* MSVC x64 intrinsic */
void __faststorefence(void)
{
    __asm__("lock; orl $0,(%rsp)");
}
#endif
