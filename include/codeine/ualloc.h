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
 * Usage example:
 * ```
 * #define UALLOC_NAME ASD
 * #define UALLOC_TYPE struct my_data
 * #incldude "uniform_allocator.h"
 *
 * ...
 * struct ualloc_ASD alloc;
 * // initialize
 * ualloc_ASD_init(&alloc);
 * // allocation
 * struct my_data* ptr = ualloc_ASD_alloc(&alloc);
 * ...
 * // deallocation
 * ualloc_ASD_free(&alloc, ptr);
 * // release memory pools
 * ualloc_ASD_destroy(&alloc);
 * ```
 *
 * Note:
 * Requires my "vector.h".
 * You can get it from <https://gist.github.com/pidhii/91a86b1c58816d0fa010083967666ed6>.
 */

#ifndef UALLOC_POOL_SIZE
#define UALLOC_POOL_SIZE 0x4000
#endif

#define _UALLOC_CONCAT(x, y) x##y
#define _UALLOC_CONCAT3(x, y, z) x##y##z
#define _UALLOC_CONCAT4(x, y, z, k) x##y##z##k
#define _UALLOC_APPLY(macro, ...) macro(__VA_ARGS__)

#define _UALLOC_CELL union _UALLOC_APPLY(_UALLOC_CONCAT, cod_ualloc_cell_, UALLOC_NAME)
#define _UALLOC_POOL struct _UALLOC_APPLY(_UALLOC_CONCAT, cod_ualloc_pool_, UALLOC_NAME)
#define _UALLOC struct _UALLOC_APPLY(_UALLOC_CONCAT, cod_ualloc_, UALLOC_NAME)
#define _UALLOC_METHOD(method) _UALLOC_APPLY(_UALLOC_CONCAT4, cod_ualloc_, UALLOC_NAME, _ , method)

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

_UALLOC_CELL
{
  UALLOC_TYPE data;
  _UALLOC_CELL* next_free;
};

_UALLOC_POOL
{
  _UALLOC_CELL* pool;
  size_t size;             /* Number of currently available free cells. */
};


#define _UALLOC_POOLS_VEC _UALLOC_APPLY(_UALLOC_CONCAT3, cod_ualloc_pools, _, UALLOC_NAME)
#define GVEC_FULL_NAME _UALLOC_POOLS_VEC
#define GVEC_TYPE _UALLOC_POOL
#include "codeine/gvec.h"

_UALLOC
{
  size_t npools;
  struct _UALLOC_POOLS_VEC pools;
  _UALLOC_POOL* curpool;
  _UALLOC_CELL* free_cell; /* Pointer to some free cell, or NULL. */
};

static __inline__
void _UALLOC_METHOD(init_pool)(_UALLOC_POOL* pool)
{
  pool->pool = malloc(UALLOC_POOL_SIZE * sizeof(_UALLOC_CELL));
  assert(pool->pool);
  pool->size = 0;
}

static
void _UALLOC_METHOD(destroy_pool)(_UALLOC_POOL* pool)
{ free(pool->pool); }

void _UALLOC_METHOD(init)(_UALLOC* ua)
{
  _UALLOC_APPLY(_UALLOC_CONCAT, _UALLOC_POOLS_VEC, _init)(&ua->pools);
  ua->curpool =
    _UALLOC_APPLY(_UALLOC_CONCAT, _UALLOC_POOLS_VEC, _push_back)(&ua->pools);
  _UALLOC_METHOD(init_pool)(ua->curpool);
}

void _UALLOC_METHOD(destroy)(_UALLOC* ua)
{
  size_t i;
  for (i = 0; i < ua->pools.size; ++i)
    _UALLOC_METHOD(destroy_pool)(&ua->pools.data[i]);
  _UALLOC_APPLY(_UALLOC_CONCAT, _UALLOC_POOLS_VEC, _destroy) (&ua->pools);
}

static __inline__
UALLOC_TYPE* _UALLOC_METHOD(alloc)(_UALLOC* ua)
{
  _UALLOC_CELL *tmp;
  if ((tmp = ua->free_cell)) {
    ua->free_cell = tmp->next_free;
    return (void*)tmp;
  }

  if (ua->curpool->size == UALLOC_POOL_SIZE) {
    /* allocate new pool */
    ua->curpool =
      _UALLOC_APPLY(_UALLOC_CONCAT, _UALLOC_POOLS_VEC, _push_back)(&ua->pools);
    _UALLOC_METHOD(init_pool)(ua->curpool);
  }

  return (void*)(ua->curpool->pool + ua->curpool->size++);
}

static __inline__
int _UALLOC_METHOD(free)(_UALLOC* ua, UALLOC_TYPE* ptr)
{
  ((_UALLOC_CELL*)ptr)->next_free = ua->free_cell;
  ua->free_cell = (void*)ptr;
  return 0;
}

#undef _UALLOC_CONCAT
#undef _UALLOC_CONCAT3
#undef _UALLOC_CONCAT4
#undef _UALLOC_APPLY

#undef _UALLOC_CELL
#undef _UALLOC_POOL
#undef _UALLOC
#undef _UALLOC_METHOD

#undef UALLOC_POOL_SIZE
#undef UALLOC_TYPE
#undef UALLOC_NAME
