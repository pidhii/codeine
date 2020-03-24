#ifndef CODEINE_COMMON_H
#define CODEINE_COMMON_H

#ifndef cod_malloc
# include <stdlib.h>
# define cod_malloc malloc
#endif

#ifndef cod_realloc
# include <stdlib.h>
# define cod_realloc realloc
#endif

#ifndef cod_free
# include <stdlib.h>
# define cod_free free
#endif

#define cod_likely(expr) __builtin_expect(!!(expr), 1)
#define cod_unlikely(expr) __builtin_expect((expr), 0)

#endif
