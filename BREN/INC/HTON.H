/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: hton.h 1.2 1995/02/22 21:37:11 sam Exp $
 * $Locker:  $
 *
 * Converting 'network' (little endian) byte order to host order
 *
 */

#ifndef _HTON_H_
#define _HTON_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Mac is in network order already
 */
#if defined(__MPW__) || defined(__THINKC__)
#define htonl(x) (x)
#define ntohl(x) (x)
#define htons(x) (x)
#define ntohs(x) (x)

typedef float net_float;

#define htonf(x) (x)
#define ntohf(x) (x)

#endif

/*
 * PC is big-endian
 */
#if defined(__GNUC__) || defined(__WATCOMC__) || defined(__ZTC__)

long bswapl(long l);
short bswaps(short s);
float bswapf(float f);

#define ntohl(x) bswapl(x)
#define htonl(x) bswapl(x)
#define ntohs(x) bswaps(x)
#define htons(x) bswaps(x)

typedef float net_float;

#define htonf(x) bswapf(x)
#define ntohf(x) bswapf(x)

#endif

#ifdef __cplusplus
};
#endif
#endif

