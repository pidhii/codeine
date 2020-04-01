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
