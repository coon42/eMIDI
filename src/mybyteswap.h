#ifndef __MY_BYTESWAP_H__
#define __MY_BYTESWAP_H__

#include <stdint.h>
#define BSWAP_16(n) ((uint16_t) ((((n) & 0xFF)   << 8) | \
                                (((n)  & 0xFF00) >> 8)))

#define BSWAP_32(n) ((uint32_t) ((((n) & 0xFF)       << 24) | \
                                 (((n) & 0xFF00)     << 8)  | \
                                 (((n) & 0xFF0000)   >> 8)  | \
                                 (((n) & 0xFF000000) >> 24)))

#endif // !__MY_BYTESWAP_H__
