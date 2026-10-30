#ifndef LEAK_HUNTER_H
# define LEAK_HUNTER_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

# define OUTPUT_FILE_NAME   "leak.log"
# define malloc(size)       lh_malloc(size, __FILE__, __LINE__)
# define free(mem_ref)      lh_free(mem_ref)

void            *lh_malloc(size_t size, const char *file, size_t line);
void            lh_free(void *mem_ref);
void            print_mem_data(void);
void            leak_report(void);

#endif /*LEAK_HUNTER_H*/
