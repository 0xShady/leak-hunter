#ifndef LEAK_HUNTER_H
# define LEAK_HUNTER_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

# undef strdup

# ifndef OUTPUT_FILE_NAME
#  define OUTPUT_FILE_NAME      "leak.log"
# endif
# define malloc(size)           lh_malloc(size, __FILE__, __LINE__)
# define calloc(count, size)    lh_calloc(count, size, __FILE__, __LINE__)
# define realloc(mem_ref, size) lh_realloc(mem_ref, size, __FILE__, __LINE__)
# define strdup(str)            lh_strdup(str, __FILE__, __LINE__)
# define free(mem_ref)          lh_free(mem_ref, __FILE__, __LINE__)

void            *lh_malloc(size_t size, const char *file, size_t line);
void            *lh_calloc(size_t count, size_t size, const char *file, size_t line);
void            *lh_realloc(void *mem_ref, size_t size, const char *file, size_t line);
char            *lh_strdup(const char *str, const char *file, size_t line);
void            lh_free(void *mem_ref, const char *file, size_t line);
void            print_mem_data(void);
void            leak_report(void);

#endif /*LEAK_HUNTER_H*/
