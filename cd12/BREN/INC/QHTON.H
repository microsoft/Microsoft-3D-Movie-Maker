/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: qhton.h 1.2 1995/02/22 21:37:20 sam Exp $
 * $Locker:  $
 *
 * Converting 'network' (little endian) byte order to host order
 *
 * This is a private implemenation - some platforms do not provide
 * this functionality. For those that do, this file should define
 * the operations in terms of the platform specific API
 */

#ifndef _QHTON_H_
#define _QHTON_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Mac is in network order already
 *
 * XXX SANE format
 */
#if defined(__MPW__) || defined(__THINKC__)
#define HTONL(x) (x)
#define NTOHL(x) (x)
#define HTONS(x) (x)
#define NTOHS(x) (x)

typedef float net_float;

#define HTONF(x) (x)
#define NTOHF(x) (x)

#endif

/*
 * PC is big-endian
 */
#if defined(__GNUC__) || defined(__WATCOMC__) || defined(__ZTC__)

long bswapl(long l);
short bswaps(short s);
float bswapf(float f);

#define NTOHL(x) bswapl(x)
#define HTONL(x) bswapl(x)
#define NTOHS(x) bswaps(x)
#define HTONS(x) bswaps(x)

typedef float net_float;

#define HTONF(x) bswapf(x)
#define NTOHF(x) bswapf(x)

#endif

#ifdef __cplusplus
};
#endif
#endif

