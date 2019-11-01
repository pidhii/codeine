#ifndef CODEINE_VEC_H
#define CODEINE_VEC_H

#include <stddef.h>
#include <stdint.h>


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

#endif
