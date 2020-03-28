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

#endif
