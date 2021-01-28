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
#ifndef COD_HASH_MAP_H
#define COD_HASH_MAP_H

#include "codeine/common.h"
#include "codeine/vec.h"

/**
 * \brief With this flag set, keys will be treated as integers, not strings.
 *
 * Whenever a function expects a key (as a string-pointer), the integer-key
 * should be casted to the pointer type, this pointer will be treated as an
 * interger.
 */
#define COD_HASH_MAP_INTKEYS 0x01

static void
cod_dummy_dtor(void* _) { }

typedef struct {
  char *key;
  void *val;
  uint32_t hash;
} cod_hash_map_elt;

typedef cod_vec(cod_hash_map_elt) cod_bucket;

typedef struct {
  int buckidx, eltidx;
} cod_hash_map_iter;

typedef struct {
  size_t size, cap;
  cod_bucket *restrict data;
  int flags;
} cod_hash_map;

cod_hash_map*
cod_hash_map_new(int flags);

void
cod_hash_map_delete(cod_hash_map *restrict map, void (*dtor)(void*));

int
cod_hash_map_insert(cod_hash_map *map, const char *key, size_t hash, void *val,
    void (*dtor)(void*));

int
cod_hash_map_insert_drain(cod_hash_map *map, char *key, size_t hash, void *val,
    void (*dtor)(void*));

int
cod_hash_map_erase(cod_hash_map *map, const char *key, size_t hash,
    void (*dtor)(void*));

cod_hash_map_elt*
cod_hash_map_find(const cod_hash_map *map, const char *key, uint32_t hash);

void
cod_hash_map_begin(const cod_hash_map *map, cod_hash_map_iter *iter);

int
cod_hash_map_next(const cod_hash_map *map, char **key, void *val,
    cod_hash_map_iter *iter);

#endif
