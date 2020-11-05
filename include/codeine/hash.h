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
#ifndef COD_HASH_H
#define COD_HASH_H

#include "codeine/common.h"

/*
 * source: http://www.cse.yorku.ca/~oz/hash.html
 */
static inline unsigned long
cod_djb2(const char *str)
{
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

/*
 * source: http://www.cse.yorku.ca/~oz/hash.html
 */
static unsigned long
cod_sdbm(const char *str)
{
  unsigned long hash = 0;
  int c;

  while ((c = *str++))
    hash = c + (hash << 6) + (hash << 16) - hash;

  return hash;
}

uint32_t
cod_halfsiphash(const uint8_t key[16], const uint8_t *m, size_t len);

#endif
