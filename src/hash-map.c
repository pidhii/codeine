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
#include "codeine/hash-map.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>

cod_hash_map*
cod_hash_map_new(int flags)
{
  cod_hash_map *map = cod_malloc(sizeof(cod_hash_map));
  map->size = 0;
  map->cap = 0x100;
  map->flags = flags;
  map->data = cod_malloc(sizeof(cod_bucket) * map->cap);
  memset(map->data, 0, sizeof(cod_bucket) * map->cap);
  return map;
}

void
cod_hash_map_delete(cod_hash_map *restrict map, void (*dtor)(void*))
{
  if (dtor == NULL)
    dtor = cod_dummy_dtor;

  for (size_t ibuck = 0; ibuck < map->cap; ++ibuck)
  {
    cod_bucket *buck = map->data + ibuck;
    if (buck->data)
    {
      for (size_t ielt = 0; ielt < buck->len; ++ielt)
      {
        cod_hash_map_elt *kv = buck->data + ielt;
        dtor(kv->val);
        cod_free(kv->key);
      }
      cod_vec_destroy(*buck);
    }
  }
  cod_free(map->data);

  cod_free(map);
}

static int
find(const cod_hash_map *map, const char *key, uint32_t hash,
    cod_hash_map_iter *iter)
{
  size_t buckidx = hash & (map->cap - 1);
  cod_bucket *buck = map->data + buckidx;

  if (buck->data == NULL)
  {
    iter->buckidx = buckidx;
    iter->eltidx = -1;
    return 0;
  }
  else
  {
    if (map->flags & COD_HASH_MAP_INTKEYS)
    {
      for (size_t i = 0; i < buck->len; ++i)
      {
        cod_hash_map_elt *elt = buck->data + i;
        if (elt->hash == hash && elt->key == key)
        {
          iter->buckidx = buckidx;
          iter->eltidx = i;
          return 1;
        }
      }
    }
    else
    {
      for (size_t i = 0; i < buck->len; ++i)
      {
        cod_hash_map_elt *elt = buck->data + i;
        if (elt->hash == hash && strcmp(elt->key, key) == 0)
        {
          iter->buckidx = buckidx;
          iter->eltidx = i;
          return 1;
        }
      }
    }

    iter->buckidx = buckidx;
    iter->eltidx = -1;
    return 0;
  }
}

cod_hash_map_elt*
cod_hash_map_find(const cod_hash_map *map, const char *key, uint32_t hash)
{
  cod_hash_map_iter iter;
  if (find(map, key, hash, &iter))
    return &map->data[iter.buckidx].data[iter.eltidx];
  else
    return NULL;
}

// TODO: don't reallocate keys
static void
rehash(cod_hash_map *map, size_t newcap)
{
  size_t oldcap = map->cap;
  cod_bucket *olddata = map->data;
  cod_bucket *newdata = cod_malloc(sizeof(cod_bucket) * newcap);
  memset(newdata, 0, sizeof(cod_bucket) * newcap);

  map->size = 0;
  map->cap = newcap;
  map->data = newdata;

  for (size_t ibuck = 0; ibuck < oldcap; ++ibuck)
  {
    cod_bucket *buck = olddata + ibuck;
    if (buck->data)
    {
      for (size_t ielt = 0; ielt < buck->len; ++ielt)
      {
        cod_hash_map_elt *elt = buck->data + ielt;
        int ok = cod_hash_map_insert_drain(map, elt->key, elt->hash, elt->val, 0);
        assert(ok);
      }
      cod_vec_destroy(*buck);
    }
  }
  free(olddata);
}

static cod_hash_map_elt*
raw_insert(cod_hash_map *map, const char *key, size_t hash, void (*dtor)(void*))
{
  cod_hash_map_iter iter;
  if (find(map, key, hash, &iter))
  {
    if (dtor == NULL) return NULL;
    cod_hash_map_elt *elt = &map->data[iter.buckidx].data[iter.eltidx];
    dtor(elt->val);
    return elt;
  }
  else
  {
    if ((map->size >> (cod_log2_u64(map->cap) - 1)) > 2)
    {
      rehash(map, map->cap << 1);
      return raw_insert(map, key, hash, dtor);
    }

    cod_bucket *buck = map->data + iter.buckidx;
    if (map->data[iter.buckidx].data == NULL)
      cod_vec_init(*buck);

    cod_hash_map_elt elt = { 0 };
    cod_vec_push(*buck, elt);
    map->size += 1;
    return &cod_vec_last(*buck);
  }
}

int
cod_hash_map_insert(cod_hash_map *map, const char *key, size_t hash, void *val,
    void (*dtor)(void*))
{
  cod_hash_map_elt *elt = raw_insert(map, key, hash, dtor);
  if (elt == NULL) return 0;

  if (map->flags & COD_HASH_MAP_INTKEYS)
  {
    elt->key = (char*)key;
  }
  else
  {
    int len = strlen(key);
    char *mykey = cod_malloc(len + 1);
    memcpy(mykey, key, len + 1);
    elt->key = mykey;
  }
  elt->hash = hash;
  elt->val = val;
  return 1;
}

int
cod_hash_map_insert_drain(cod_hash_map *map, char *key, size_t hash, void *val,
    void (*dtor)(void*))
{
  cod_hash_map_elt *elt = raw_insert(map, key, hash, dtor);
  if (elt == NULL) return 0;

  elt->key = key;
  elt->hash = hash;
  elt->val = val;
  return 1;
}

int
cod_hash_map_erase(cod_hash_map *map, const char *key, size_t hash,
    void (*dtor)(void*))
{
  if (dtor == NULL)
    dtor = cod_dummy_dtor;

  cod_hash_map_iter iter;
  if (find(map, key, hash, &iter))
  {
    cod_bucket *buck = map->data + iter.buckidx;
    cod_hash_map_elt *elt = buck->data + iter.eltidx;
    cod_free(elt->key);
    dtor(elt->val);
    cod_vec_erase(*buck, iter.eltidx);
    return 1;
  }
  else
  {
    return 0;
  }
}

static int
next_bucket(const cod_hash_map *map, int from)
{
  for (size_t i = from; i < map->cap; ++i)
  {
    cod_bucket *buck = map->data + i;
    if (buck->data && buck->len > 0)
      return i;
  }
  return -1;
}

void
cod_hash_map_begin(const cod_hash_map *map, cod_hash_map_iter *iter)
{
  int ib = next_bucket(map, 0);
  iter->buckidx = ib;
  iter->eltidx = 0;
}

int
cod_hash_map_next(const cod_hash_map *map, char **key, void *val,
    cod_hash_map_iter *iter)
{
  if (iter->buckidx < 0)
    return 0;

  cod_bucket *buck = map->data + iter->buckidx;

  if (key) *key = buck->data[iter->eltidx].key;
  if (val) *(void**)val = buck->data[iter->eltidx].val;

  if (++iter->eltidx == (int)buck->len)
  {
    iter->buckidx = next_bucket(map, iter->buckidx + 1);
    iter->eltidx = 0;
  }
  return 1;
}
