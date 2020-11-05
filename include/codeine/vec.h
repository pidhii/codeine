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
#ifndef CODEINE_VEC_H
#define CODEINE_VEC_H

#include <stddef.h>
#include <stdint.h>

#include "codeine/common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define cod_vec(T) struct { T* data; size_t len, cap; }

#define cod_vec_value_type(vec) typeof(*(vec).data)
#define cod_vec_value_size(vec) sizeof(cod_vec_value_type(vec))

#define cod_vec_init(vec) \
  do {                    \
    (vec).cap = 0;        \
    (vec).len = 0;        \
    (vec).data = NULL;    \
  } while (0)

#define cod_vec_init_with_cap(vec, c)                        \
  do {                                                       \
    (vec).cap = (c);                                         \
    if ((vec).cap == 0)                                      \
      (vec).cap = 0x10;                                      \
    (vec).len = 0;                                           \
    (vec).data = cod_malloc(cod_vec_value_size(vec) * 0x10); \
  } while (0)

#define cod_vec_destroy(vec) \
  do {                       \
    if ((vec).data)          \
      cod_free((vec).data);  \
    (vec).data = NULL;       \
    (vec).len = 0;           \
    (vec).cap = 0;           \
  } while (0)

#define cod_vec_reserve1(vec)                                                    \
  do {                                                                           \
    if (cod_unlikely((vec).len == (vec).cap)) {                                  \
      if (cod_likely((vec).cap))                                                 \
        (vec).cap <<= 1;                                                         \
      else                                                                       \
        (vec).cap = 0x10;                                                        \
      (vec).data = cod_realloc((vec).data, cod_vec_value_size(vec) * (vec).cap); \
    }                                                                            \
  } while (0)

#define cod_vec_push(vec, x...)       \
  do {                             \
    cod_vec_reserve1(vec);         \
    (vec).data[(vec).len++] = (x); \
  } while (0)

#define cod_vec_emplace(vec, ctor...)                         \
  do {                                                        \
    cod_vec_reserve1(vec);                                    \
    (vec).data[(vec).len++] = (cod_vec_value_type(vec)) ctor; \
  } while (0)

#define cod_vec_pop(vec) ((vec).data[--(vec).len])

#define cod_vec_last(vec) ((vec).data[(vec).len - 1])
#define cod_vec_last_ptr(vec) (&(vec).data[(vec).len - 1])

#define cod_vec_iter(vec, i_ident, x_ident, body...)           \
  do {                                                         \
    for (size_t i_ident = 0; i_ident < (vec).len; ++i_ident) { \
      cod_vec_value_type(vec) x_ident = (vec).data[i_ident];   \
      body;                                                    \
    }                                                          \
  } while (0)

#define cod_vec_riter(vec, i_ident, x_ident, body...)            \
  do {                                                           \
    for (int i_ident = (vec).len - 1; i_ident >= 0; --i_ident) { \
      cod_vec_value_type(vec) x_ident = (vec).data[i_ident];     \
      body;                                                      \
    }                                                            \
  } while (0)

#define cod_vec_insert(vec, x, k)                     \
  do {                                                \
    if (k == (vec).len) {                             \
      cod_vec_push((vec), x);                         \
    } else {                                          \
      cod_vec_reserve1(vec);                          \
      memmove((vec).data + k + 1, (vec).data + k,     \
          cod_vec_value_size(vec) * ((vec).len - k)); \
      (vec).data[k] = x;                              \
      (vec).len += 1;                                 \
    }                                                 \
  } while (0)

#define cod_vec_erase(vec, k)                       \
  do {                                              \
    memmove((vec).data + k, (vec).data + k + 1,     \
        cod_vec_value_size(vec) * ((vec).len - k)); \
    (vec).len -= 1;                                 \
  } while (0)

#define cod_vec_append(vec, begin, end)                                    \
  do {                                                                     \
    for (const cod_vec_value_type(vec) *iter = begin; iter != end; ++iter) \
      cod_vec_push(vec, *iter);                                            \
  } while (0)


struct cod_strvec {
  char **data;
  size_t size;
  size_t cap;
};

void
cod_strvec_init(struct cod_strvec *vec);

void
cod_strvec_destroy(struct cod_strvec *vec);

void
cod_strvec_push(struct cod_strvec *vec, const char *str);

void
cod_strvec_pop(struct cod_strvec *vec);

void
cod_strvec_insert(struct cod_strvec *vec, const char *str, size_t at);

long long int
cod_strvec_find(struct cod_strvec *vec, const char *str);

long long int
cod_strvec_rfind(struct cod_strvec *vec, const char *str);


struct cod_intvec {
  intmax_t *data;
  size_t size;
  size_t cap;
};

void
cod_intvec_init(struct cod_intvec *vec);

void
cod_intvec_destroy(struct cod_intvec *vec);

void
cod_intvec_push(struct cod_intvec *vec, intmax_t x);

void
cod_intvec_pop(struct cod_intvec *vec, size_t n);

void
cod_intvec_insert(struct cod_intvec *vec, intmax_t x, size_t at);

long long int
cod_intvec_find(struct cod_intvec *vec, intmax_t x);

long long int
cod_intvec_rfind(struct cod_intvec *vec, intmax_t x);


struct cod_ptrvec {
  void **data;
  size_t size;
  size_t cap;
};

void
cod_ptrvec_init(struct cod_ptrvec *vec);

void
cod_ptrvec_destroy(struct cod_ptrvec *vec, void (*free)(void*));

void
cod_ptrvec_push(struct cod_ptrvec *vec, void *ptr, void* (*copy)(void*));

void
cod_ptrvec_pop(struct cod_ptrvec *vec, void (*free)(void*));

void
cod_ptrvec_insert(struct cod_ptrvec *vec, void *str, size_t at, void* (*copy)(void*));

#ifdef __cplusplus
}
#endif

#endif
