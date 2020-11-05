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
 * Generic vector.
 *
 * USAGE:
 * 1) #define VECTOR_TYPE and VECTOR_NAME:
 *    - VECTOR_TYPE is the type of data to be stored inside vector
 *    - VECTOR_NAME will be inserted inside type- and method-names as:
 *       . type: struct vector_<VECTOR_NAME>;
 *       . methods: vector_<VECTOR_NAME>_<method-name>(...)
 * 
 * 2) #include "vector.h"
 *    Note: it will #undef VECTOR_TYPE and VECTOR_NAME on it's own.
 *
 * 3) Make Love.
 *
 *
 * METHODS:
 * + Construct/Destroy:
 *   - vector_XXX_init(vec)       : Initialize vector.
 *   - vector_XXX_destroy(vec)    : Release memory.
 *
 * + Accessors:
 *   - vector_XXX_get(vec, idx)   : Get element at given index.
 *   - vector_XXX_at(vec, idx)    : The same, but also check for overflow.
 *   - vector_XXX_fornt(vec)      : Check if vector is not empty and return first element.
 *   - vector_XXX_back(vec)       : Check if vector is not empty and return last element.
 *
 * + Pleasure:
 *   - vector_XXX_push_back(vec)  : Increment size of vector and return pointer to new element.
 *                                  Note: initialisation of the new elemnt is on user responsibility.
 *   - vector_XXX_push_back_val(vec, val) : Same, but copies the value from `val`.
 *   - vector_XXX_pop_back(vec)   : Decrement size of vector.
 *                                  Note #1: deinitialisation of the elemnt to
 *                                           be removed is on user responsibility.
 *                                  Note #2: memory will never be freed by push,
 *                                           in order to do so, use `shrink`.
 *
 * + Buffer manipulations:
 *   - vector_XXX_reserve(vec, n) : Allocate space (if needed) to be able to hold
 *                                  at least n-more values without additional allocations,
 *   - vector_XXX_resize(vec, n)  : The same, but also advance the size of the vector,
 *   - vector_XXX_shrink(vec)     : Release (maybe) unneeded memory, so the capacity
 *                                  will be equal to its size,
 */
#if (!defined(GVEC_TYPE) || !(defined(GVEC_NAME) || defined(GVEC_FULL_NAME)))
#error Before including "gvec.h" you must define GVEC_TYPE and GVEC[_FULL]_NAME
#endif

#define _GVEC_ELSZ sizeof(GVEC_TYPE)

#define _GVEC_CONCAT(x, y) x##y
#define _GVEC_CONCAT3(x, y, z) x##y##z
#define _GVEC_CONCAT4(x, y, z, k) x##y##z##k
#define _GVEC_APPLY(macro, ...) macro(__VA_ARGS__)

#ifdef GVEC_FULL_NAME
#define _GVEC_METHOD(method) _GVEC_APPLY(_GVEC_CONCAT3, GVEC_FULL_NAME, _ , method)
#define _GVEC GVEC_FULL_NAME
#else
#define _GVEC_METHOD(method) _GVEC_APPLY(_GVEC_CONCAT4, cod_gvec_, GVEC_NAME, _ , method)
#define _GVEC _GVEC_APPLY(_GVEC_CONCAT, cod_gvec_, GVEC_NAME)
#endif

#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct _GVEC
{
  GVEC_TYPE* data;
  size_t       size;
  size_t       cap;
};

static
void _GVEC_METHOD(init)(struct _GVEC* vec);
static
void _GVEC_METHOD(destroy)(struct _GVEC* vec);

static __inline__
GVEC_TYPE* _GVEC_METHOD(get)(struct _GVEC* vec, size_t idx);
static __inline__
GVEC_TYPE* _GVEC_METHOD(at)(struct _GVEC* vec, size_t idx);
static __inline__
GVEC_TYPE* _GVEC_METHOD(front)(struct _GVEC* vec);
static __inline__
GVEC_TYPE* _GVEC_METHOD(back)(struct _GVEC* vec);
static __inline__
GVEC_TYPE* _GVEC_METHOD(push_back)(struct _GVEC* vec);
static __inline__
GVEC_TYPE* _GVEC_METHOD(push_front)(struct _GVEC* vec);
static __inline__
void _GVEC_METHOD(push_back_val)(struct _GVEC* vec, GVEC_TYPE val);
static __inline__
void _GVEC_METHOD(push_front_val)(struct _GVEC* vec, GVEC_TYPE val);
static __inline__
void _GVEC_METHOD(pop_back)(struct _GVEC* vec);
static __inline__
void _GVEC_METHOD(pop_front)(struct _GVEC* vec);
static __inline__
void _GVEC_METHOD(reserve)(struct _GVEC* vec, size_t n);
static __inline__
void _GVEC_METHOD(resize)(struct _GVEC* vec, size_t n);
static __inline__
void _GVEC_METHOD(shrink)(struct _GVEC* vec);


static
void _GVEC_METHOD(init)(struct _GVEC* vec)
{
  /* Allocate one element by default,
   * so to avoid special cases in operations.  */
  vec->data = malloc(_GVEC_ELSZ);
  vec->size = 0;
  vec->cap = 1;
}

static
void _GVEC_METHOD(destroy)(struct _GVEC* vec)
{ free(vec->data); }

static __inline__
void _GVEC_METHOD(reserve_one_more)(struct _GVEC* vec)
{
  if (vec->cap == vec->size) {
    vec->cap <<= 1;
    vec->data = realloc(vec->data, vec->cap * _GVEC_ELSZ);
    assert(vec->data);
  }
}

static __inline__
void _GVEC_METHOD(prepare_push_front)(struct _GVEC* vec)
{
  GVEC_TYPE* nwbuf;
  if (vec->cap == vec->size) {
    vec->cap <<= 1;
    nwbuf = malloc(vec->cap);
    assert(nwbuf);
    memcpy(nwbuf + 1, vec->data, vec->size * _GVEC_ELSZ);
    free(vec->data);
    vec->data = nwbuf;
  } else {
    memmove(vec->data + 1, vec->data, vec->size * _GVEC_ELSZ);
  }
  ++vec->size;
}

static __inline__
GVEC_TYPE* _GVEC_METHOD(push_back)(struct _GVEC* vec)
{
  _GVEC_METHOD(reserve_one_more)(vec);
  return vec->data + vec->size++;
}

static __inline__
GVEC_TYPE* _GVEC_METHOD(push_front)(struct _GVEC* vec)
{
  _GVEC_METHOD(prepare_push_front)(vec);
  return vec->data;
}

static __inline__
void _GVEC_METHOD(push_back_val)(struct _GVEC* vec, GVEC_TYPE val)
{
  _GVEC_METHOD(reserve_one_more)(vec);
  vec->data[vec->size++] = val;
}

static __inline__
void _GVEC_METHOD(push_front_val)(struct _GVEC* vec, GVEC_TYPE val)
{
  _GVEC_METHOD(prepare_push_front)(vec);
  vec->data[0] = val;
}

static __inline__
GVEC_TYPE* _GVEC_METHOD(front)(struct _GVEC* vec)
{
  assert(vec->size);
  return vec->data;
}

static __inline__
GVEC_TYPE* _GVEC_METHOD(bask)(struct _GVEC* vec)
{
  assert(vec->size);
  return vec->data + vec->size - 1;
}

static __inline__
void _GVEC_METHOD(pop_back)(struct _GVEC* vec)
{
  assert(vec->size > 0);
  vec->size--;
}

static __inline__
void _GVEC_METHOD(pop_front)(struct _GVEC* vec)
{
  assert(vec->size > 0);
  memmove(vec->data, vec->data + 1, vec->size * _GVEC_ELSZ);
}

static __inline__
void _GVEC_METHOD(shrink)(struct _GVEC* vec)
{
  vec->cap = vec->size;
  vec->data = realloc(vec->data, vec->cap * _GVEC_ELSZ);
}

static __inline__
void _GVEC_METHOD(reserve)(struct _GVEC* vec, size_t n)
{
  if (vec->cap >= _GVEC_ELSZ * n)
    return;

  /* Round up to the next power of 2. */
  size_t sz = n;
  sz--;
  sz |= sz >> 1;
  sz |= sz >> 2;
  sz |= sz >> 4;
  sz |= sz >> 8;
  sz |= sz >> 16;
  sz |= sz >> 32;
  sz++;
  vec->cap = sz;
  vec->data = realloc(vec->data, sz * _GVEC_ELSZ);
}

static __inline__
void _GVEC_METHOD(resize)(struct _GVEC* vec, size_t n)
{
  _GVEC_METHOD(reserve)(vec, n);
  vec->size = n;
}

static __inline__
GVEC_TYPE* _GVEC_METHOD(get)(struct _GVEC* vec, size_t idx)
{ return vec->data + idx; }

static __inline__
GVEC_TYPE* _GVEC_METHOD(at)(struct _GVEC* vec, size_t idx)
{
  assert(idx < vec->size);
  return vec->data + idx;
}

#undef GVEC_TYPE
#ifdef GVEC_NAME
#undef GVEC_NAME
#endif
#ifdef GVEC_FULL_NAME
#undef GVEC_FULL_NAME
#endif
#undef _GVEC_ELSZ
#undef _GVEC_CONCAT
#undef _GVEC_CONCAT3
#undef _GVEC_CONCAT4
#undef _GVEC_APPLY
#undef _GVEC_METHOD
#undef _GVEC
