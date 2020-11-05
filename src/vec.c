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
#include "codeine/vec.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

void
cod_strvec_init(struct cod_strvec *vec)
{
  vec->cap = 0x10;
  vec->size = 0;
  vec->data = malloc(sizeof(char*) * vec->cap);
}

void
cod_strvec_destroy(struct cod_strvec *vec)
{
  while (vec->size--)
    free(vec->data[vec->size]);
  free(vec->data);
}

void
cod_strvec_push(struct cod_strvec *vec, const char *str)
{
  if (vec->size == vec->cap) {
    vec->cap <<= 1;
    vec->data = realloc(vec->data, sizeof(char*) * vec->cap);
  }
  vec->data[vec->size++] = strdup(str);
}

void
cod_strvec_pop(struct cod_strvec *vec)
{ free(vec->data[--vec->size]); }

void
cod_strvec_insert(struct cod_strvec *vec, const char *str, size_t at)
{
  if (at == vec->size) {
    cod_strvec_push(vec, str);
  } else {
    if (vec->size == vec->cap) {
      vec->cap <<= 1;
      vec->data = realloc(vec->data, sizeof(char*) * vec->cap);
    }
    memmove(vec->data + at + 1, vec->data + at, sizeof(char*) * (vec->size - at));
    vec->data[at] = strdup(str);
    vec->size += 1;
  }
}

long long int
cod_strvec_find(struct cod_strvec *vec, const char *str)
{
  for (size_t i = 0; i < vec->size; ++i) {
    if (strcmp(vec->data[i], str) == 0)
      return i;
  }
  return -1;
}

long long int
cod_strvec_rfind(struct cod_strvec *vec, const char *str)
{
  for (ssize_t i = vec->size - 1; i >= 0; --i) {
    if (strcmp(vec->data[i], str) == 0)
      return i;
  }
  return -1;
}


void
cod_intvec_init(struct cod_intvec *vec)
{
  vec->cap = 0x10;
  vec->size = 0;
  vec->data = malloc(sizeof(intmax_t) * vec->cap);
}

void
cod_intvec_destroy(struct cod_intvec *vec)
{ free(vec->data); }

void
cod_intvec_push(struct cod_intvec *vec, intmax_t x)
{
  if (vec->size == vec->cap) {
    vec->cap <<= 1;
    vec->data = realloc(vec->data, sizeof(intmax_t) * vec->cap);
  }
  vec->data[vec->size++] = x;
}

void
cod_intvec_pop(struct cod_intvec *vec, size_t n)
{
  assert(n <= vec->size);
  vec->size -= n;
}

void
cod_intvec_insert(struct cod_intvec *vec, intmax_t x, size_t at)
{
  if (at == vec->size) {
    cod_intvec_push(vec, x);
  } else {
    if (vec->size == vec->cap) {
      vec->cap <<= 1;
      vec->data = realloc(vec->data, sizeof(intmax_t) * vec->cap);
    }
    memmove(vec->data + at + 1, vec->data + at, sizeof(intmax_t) * (vec->size - at));
    vec->data[at] = x;
    vec->size += 1;
  }
}

long long int
cod_intvec_find(struct cod_intvec *vec, intmax_t x)
{
  for (size_t i = 0; i < vec->size; ++i) {
    if (vec->data[i] == x)
      return i;
  }
  return -1;
}

long long int
cod_intvec_rfind(struct cod_intvec *vec, intmax_t x)
{
  for (ssize_t i = vec->size - 1; i >= 0; --i) {
    if (vec->data[i] == x)
      return i;
  }
  return -1;
}


void
cod_ptrvec_init(struct cod_ptrvec *vec)
{
  vec->cap = 0x10;
  vec->size = 0;
  vec->data = malloc(sizeof(void*) * vec->cap);
}

void
cod_ptrvec_destroy(struct cod_ptrvec *vec, void (*delete)(void*))
{
  if (delete) {
    while (vec->size--)
      delete(vec->data[vec->size]);
  }
  free(vec->data);
}

void
cod_ptrvec_push(struct cod_ptrvec *vec, void *ptr, void* (*copy)(void*))
{
  if (vec->size == vec->cap) {
    vec->cap <<= 1;
    vec->data = realloc(vec->data, sizeof(char*) * vec->cap);
  }
  vec->data[vec->size++] = copy ? copy(ptr) : ptr;
}

void
cod_ptrvec_pop(struct cod_ptrvec *vec, void (*delete)(void*))
{
  if (delete)
    delete(vec->data[vec->size - 1]);
  vec->size -= 1;
}

void
cod_ptrvec_insert(struct cod_ptrvec *vec, void *ptr, size_t at, void* (*copy)(void*))
{
  if (at == vec->size) {
    cod_ptrvec_push(vec, ptr, copy);
  } else {
    if (vec->size == vec->cap) {
      vec->cap <<= 1;
      vec->data = realloc(vec->data, sizeof(void*) * vec->cap);
    }
    memmove(vec->data + at + 1, vec->data + at, sizeof(void*) * (vec->size - at));
    vec->data[at] = copy ? copy(ptr) : ptr;
    vec->size += 1;
  }
}
