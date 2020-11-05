/* Copyright (C) 2020  Ivan Pidhurskyi
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
/*
 * source: https://github.com/rurban/smhasher/blob/master/halfsiphash.c
 */
#include "codeine/hash.h"

#include <stdint.h>
#include <stddef.h>

#define ROTL(x, b) (uint32_t)(((x) << (b)) | ((x) >> (32 - (b))))
#define U8TO32_LE(p)                                                    \
  (((uint32_t)((p)[0])) | ((uint32_t)((p)[1]) << 8) |                 \
   ((uint32_t)((p)[2]) << 16) | ((uint32_t)((p)[3]) << 24))
#define SIPROUND                                       \
  do {                                               \
    v0 += v1;                                      \
    v1 = ROTL(v1, 5);                              \
    v1 ^= v0;                                      \
    v0 = ROTL(v0, 16);                             \
    v2 += v3;                                      \
    v3 = ROTL(v3, 8);                              \
    v3 ^= v2;                                      \
    v0 += v3;                                      \
    v3 = ROTL(v3, 7);                              \
    v3 ^= v0;                                      \
    v2 += v1;                                      \
    v1 = ROTL(v1, 13);                             \
    v1 ^= v2;                                      \
    v2 = ROTL(v2, 16);                             \
  } while (0)

/* the faster half 32bit variant for the linux kernel */
uint32_t
cod_halfsiphash(const uint8_t key[16], const uint8_t *m, size_t len)
{
  uint32_t v0 = 0;
  uint32_t v1 = 0;
  uint32_t v2 = 0x6c796765;
  uint32_t v3 = 0x74656462;
  uint32_t k0 = U8TO32_LE(key);
  uint32_t k1 = U8TO32_LE(key + 8);
  uint32_t mi;
  const uint8_t *end = m + len - (len % sizeof(uint32_t));
  const int left = len & 3;
  uint32_t b = ((uint32_t)len) << 24;
  v3 ^= k1;
  v2 ^= k0;
  v1 ^= k1;
  v0 ^= k0;

  for (; m != end; m += 4) {
    mi = U8TO32_LE(m);
    v3 ^= mi;
    SIPROUND;
    SIPROUND;
    v0 ^= mi;
  }

  switch (left) {
    case 3:
      b |= ((uint32_t)m[2]) << 16;
      __attribute__((fallthrough));
    case 2:
      b |= ((uint32_t)m[1]) << 8;
      __attribute__((fallthrough));
    case 1:
      b |= ((uint32_t)m[0]);
      break;
    case 0:
      break;
  }

  v3 ^= b;
  SIPROUND;
  SIPROUND;
  v0 ^= b;
  v2 ^= 0xff;
  SIPROUND;
  SIPROUND;
  SIPROUND;
  SIPROUND;
  return v1 ^ v3;
}
