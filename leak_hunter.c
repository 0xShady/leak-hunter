#include "leak_hunter.h"

#undef  malloc
#undef  calloc
#undef  realloc
#undef  strdup
#undef  free

typedef struct s_lh_block
{
	void				*address;
	size_t				size;
	const char			*file;
	size_t				line;
	struct s_lh_block	*next;
}	t_lh_block;

typedef struct s_lh_list
{
	t_lh_block	*head;
	t_lh_block	*tail;
}	t_lh_list;

static t_lh_list	g_live = {NULL, NULL};
static t_lh_list	g_bad = {NULL, NULL};

static void	lh_add(t_lh_list *list, void *address, size_t size,
		const char *file, size_t line)
{
	t_lh_block	*block;

	if (!(block = malloc(sizeof(t_lh_block))))
		return ;
	block->address = address;
	block->size = size;
	block->file = file;
	block->line = line;
	block->next = NULL;
	if (list->tail)
		list->tail->next = block;
	else
		list->head = block;
	list->tail = block;
}

static t_lh_block	*lh_find(t_lh_list *list, void *address)
{
	t_lh_block	*block;

	block = list->head;
	while (block && block->address != address)
		block = block->next;
	return (block);
}

static int	lh_remove(t_lh_list *list, void *address)
{
	t_lh_block	*block;
	t_lh_block	*prev;

	prev = NULL;
	block = list->head;
	while (block && block->address != address)
	{
		prev = block;
		block = block->next;
	}
	if (!block)
		return (0);
	if (prev)
		prev->next = block->next;
	else
		list->head = block->next;
	if (list->tail == block)
		list->tail = prev;
	free(block);
	return (1);
}

void	*lh_malloc(size_t size, const char *file, size_t line)
{
	void	*ptr;

	ptr = malloc(size);
	if (ptr != NULL)
		lh_add(&g_live, ptr, size, file, line);
	return (ptr);
}

void	*lh_calloc(size_t count, size_t size, const char *file, size_t line)
{
	void	*ptr;

	ptr = calloc(count, size);
	if (ptr != NULL)
		lh_add(&g_live, ptr, count * size, file, line);
	return (ptr);
}

char	*lh_strdup(const char *str, const char *file, size_t line)
{
	char	*ptr;

	ptr = strdup(str);
	if (ptr != NULL)
		lh_add(&g_live, ptr, strlen(ptr) + 1, file, line);
	return (ptr);
}

void	*lh_realloc(void *mem_ref, size_t size, const char *file, size_t line)
{
	t_lh_block	*block;
	void		*ptr;

	if (mem_ref == NULL)
		return (lh_malloc(size, file, line));
	if (!(block = lh_find(&g_live, mem_ref)))
	{
		lh_add(&g_bad, mem_ref, 0, file, line);
		return (NULL);
	}
	if (size == 0)
	{
		lh_free(mem_ref, file, line);
		return (NULL);
	}
	ptr = realloc(mem_ref, size);
	if (ptr != NULL)
	{
		block->address = ptr;
		block->size = size;
		block->file = file;
		block->line = line;
	}
	return (ptr);
}

void	lh_free(void *mem_ref, const char *file, size_t line)
{
	if (mem_ref == NULL)
		return ;
	if (lh_remove(&g_live, mem_ref))
		free(mem_ref);
	else
		lh_add(&g_bad, mem_ref, 0, file, line);
}

void	print_mem_data(void)
{
	t_lh_block	*tmp;

	tmp = g_live.head;
	while (tmp)
	{
		printf("address: %p\n", tmp->address);
		printf("size: %zu\n", tmp->size);
		printf("file: %s\n", tmp->file);
		printf("line: %zu\n", tmp->line);
		tmp = tmp->next;
	}
}

static void	lh_print_blocks(FILE *log, t_lh_block *block, int with_size)
{
	while (block)
	{
		fprintf(log, "│ address: %20p │\n", block->address);
		if (with_size)
			fprintf(log, "│ size (bytes): %15zu │\n", block->size);
		fprintf(log, "│ file: %23s │\n", block->file);
		fprintf(log, "│ line: %23zu │\n", block->line);
		if (block->next)
			fprintf(log, "├───────────────────────────────┤\n");
		block = block->next;
	}
}

void	leak_report(void)
{
	t_lh_block	*tmp;
	FILE		*log;
	size_t		leaks_size;
	size_t		allocated_blocks;
	size_t		bad_frees;

	leaks_size = 0;
	allocated_blocks = 0;
	bad_frees = 0;
#ifdef LEAK_HUNTER_STDERR
	log = stderr;
#else
	if (!(log = fopen(OUTPUT_FILE_NAME, "w")))
		return ;
#endif
	tmp = g_live.head;
	while (tmp)
	{
		allocated_blocks++;
		leaks_size += tmp->size;
		tmp = tmp->next;
	}
	tmp = g_bad.head;
	while (tmp)
	{
		bad_frees++;
		tmp = tmp->next;
	}
	fprintf(log, "┌─────────┤ LEAKS LOG ├─────────┐\n");
	fprintf(log, "│ Total leaks found: %10zu │\n", allocated_blocks);
	fprintf(log, "│ Total leaks size: %11zu │\n", leaks_size);
	fprintf(log, "├───────────────────────────────┤\n");
	if (g_live.head)
		lh_print_blocks(log, g_live.head, 1);
	else
		fprintf(log, "│   your code is leaks free!!   │\n");
	if (g_bad.head)
	{
		fprintf(log, "├───────────────────────────────┤\n");
		fprintf(log, "│ Bad frees found: %12zu │\n", bad_frees);
		fprintf(log, "├───────────────────────────────┤\n");
		lh_print_blocks(log, g_bad.head, 0);
	}
	fprintf(log, "└───────────────────────────────┘\n");
#ifndef LEAK_HUNTER_STDERR
	fclose(log);
#endif
}
