#ifndef COD_HASH_MAP_H
#define COD_HASH_MAP_H

#include "codeine/common.h"
#include "codeine/vec.h"

static void
cod_dummy_dtor(void* _) { }

typedef struct {
  size_t hash;
  char *key;
  void *val;
} cod_hash_map_elt;

typedef cod_vec(cod_hash_map_elt) cod_bucket;

typedef struct {
  int buckidx, eltidx;
} cod_hash_map_iter;

typedef struct {
  size_t size, cap;
  cod_bucket *restrict data;
} cod_hash_map;

cod_hash_map*
cod_hash_map_new(void);

void
cod_hash_map_delete(cod_hash_map *restrict map, void (*dtor)(void*));

int
cod_hash_map_insert(cod_hash_map *map, const char *key, size_t hash, void *val,
    void (*dtor)(void*));

int
cod_hash_map_erase(cod_hash_map *map, const char *key, size_t hash,
    void (*dtor)(void*));

cod_hash_map_elt*
cod_hash_map_find(const cod_hash_map *map, const char *key, uint64_t hash);

#endif