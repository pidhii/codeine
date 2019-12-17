#ifndef CODEINE_VEC_H
#define CODEINE_VEC_H

#include <stddef.h>
#include <stdint.h>

#define cod_likely(expr) __builtin_expect(!!(expr), 1)
#define cod_unlikely(expr) __builtin_expect((expr), 0)

#if !defined(cod_malloc)
# include <stdlib.h>
# define cod_malloc malloc
# define cod_free free
# define cod_realloc realloc
#elif !defined(cod_free) || !defined(cod_realloc)
# error "undefined cod_free(void*) || cod_realloc(void*,size_t)"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define cod_vec(T) struct { T* restrict data; size_t len, cap; }

#define cod_vec_value_type(vec) typeof(*(vec).data)
#define cod_vec_value_size(vec) sizeof(cod_vec_value_type(vec))

#define cod_vec_init(vec)                                    \
  do {                                                       \
    (vec).cap = 0x10;                                        \
    (vec).len = 0;                                           \
    (vec).data = cod_malloc(cod_vec_value_size(vec) * 0x10); \
  } while (0)

#define cod_vec_destroy(vec) \
  cod_free((vec).data)

#define cod_vec_reserve1(vec)                                                    \
  do {                                                                           \
    if (cod_unlikely((vec).len == (vec).cap)) {                                  \
      (vec).cap <<= 1;                                                           \
      (vec).data = cod_realloc((vec).data, cod_vec_value_size(vec) * (vec).cap); \
    }                                                                            \
  } while (0)

#define cod_vec_push(vec, x)       \
  do {                             \
    cod_vec_reserve1(vec);         \
    (vec).data[(vec).len++] = (x); \
  } while (0)

#define cod_vec_pop(vec) ((vec).data[--(vec).len])

#define cod_vec_iter(vec, i_ident, x_ident, body)              \
  do {                                                         \
    for (size_t i_ident = 0; i_ident < (vec).len; ++i_ident) { \
      cod_vec_value_type(vec) x_ident = (vec).data[i_ident];   \
      body;                                                    \
    }                                                          \
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
