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
#ifndef CODEINE_COMMON_H
#define CODEINE_COMMON_H

#ifndef cod_malloc
# include <stdlib.h>
# define cod_malloc malloc
#endif

#ifndef cod_realloc
# include <stdlib.h>
# define cod_realloc realloc
#endif

#ifndef cod_free
# include <stdlib.h>
# define cod_free free
#endif

#define cod_likely(expr) __builtin_expect(!!(expr), 1)
#define cod_unlikely(expr) __builtin_expect((expr), 0)

#include <stdint.h>

static inline uint8_t __attribute__((pure))
cod_log2_u64(uint64_t x)
{
  return 64 - __builtin_clzll(x);
}

static inline uint8_t __attribute__((pure))
cod_log2_u32(uint32_t x)
{
  return 32 - __builtin_clzl(x);
}

static inline uint32_t
cod_rndup2_u32(uint32_t v)
{
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v;
}

static inline uint64_t
cod_rndup2_u64(uint64_t v)
{
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v |= v >> 32;
  v++;
  return v;
}

#endif
