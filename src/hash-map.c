#include "codeine/hash-map.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>

cod_hash_map*
cod_hash_map_new(void)
{
  cod_hash_map *map = cod_malloc(sizeof(cod_hash_map));
  map->size = 0;
  map->cap = 0x100;
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
        cod_free(kv->key);
        dtor(kv->val);
      }
      cod_vec_destroy(*buck);
    }
  }
  cod_free(map->data);

  cod_free(map);
}

static int
find(const cod_hash_map *map, const char *key, uint64_t hash,
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

    iter->buckidx = buckidx;
    iter->eltidx = -1;
    return 0;
  }
}

cod_hash_map_elt*
cod_hash_map_find(const cod_hash_map *map, const char *key, uint64_t hash)
{
  cod_hash_map_iter iter;
  if (find(map, key, hash, &iter))
    return &map->data[iter.buckidx].data[iter.eltidx];
  else
    return NULL;
}

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
        int ok = cod_hash_map_insert(map, elt->key, elt->hash, elt->val, 0);
        assert(ok);
        free(elt->key);
      }
      cod_vec_destroy(*buck);
    }
  }
  free(olddata);
}

int
cod_hash_map_insert(cod_hash_map *map, const char *key, size_t hash, void *val,
    void (*dtor)(void*))
{
  cod_hash_map_iter iter;
  if (find(map, key, hash, &iter))
  {
    if (dtor == NULL) return 0;
    cod_hash_map_elt *elt = &map->data[iter.buckidx].data[iter.eltidx];
    dtor(elt->val);
    elt->val = val;
    return 1;
  }
  else
  {
    if ((map->size >> (cod_log2_u64(map->cap) - 1)) > 2)
    {
      rehash(map, map->cap << 1);
      return cod_hash_map_insert(map, key, hash, val, dtor);
    }

    cod_bucket *buck = map->data + iter.buckidx;
    if (map->data[iter.buckidx].data == NULL)
      cod_vec_init(*buck);

    int len = strlen(key);
    char *mykey = cod_malloc(len + 1);
    memcpy(mykey, key, len + 1);
    cod_hash_map_elt elt = { .hash = hash, .key = mykey, .val = val };
    cod_vec_push(*buck, elt);
    map->size += 1;
    return 1;
  }
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
