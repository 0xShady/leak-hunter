#include "leak_hunter.h"

#undef  malloc
#undef  free

typedef struct s_lh_block
{
	void				*address;
	size_t				size;
	const char			*file;
	size_t				line;
	struct s_lh_block	*next;
}	t_lh_block;

static t_lh_block	*g_head = NULL;
static t_lh_block	*g_tail = NULL;

static void	lh_add(void *address, size_t size, const char *file, size_t line)
{
	t_lh_block	*block;

	if (!(block = malloc(sizeof(t_lh_block))))
		return ;
	block->address = address;
	block->size = size;
	block->file = file;
	block->line = line;
	block->next = NULL;
	if (g_tail)
		g_tail->next = block;
	else
		g_head = block;
	g_tail = block;
}

static int	lh_remove(void *address)
{
	t_lh_block	*block;
	t_lh_block	*prev;

	prev = NULL;
	block = g_head;
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
		g_head = block->next;
	if (g_tail == block)
		g_tail = prev;
	free(block);
	return (1);
}

void	*lh_malloc(size_t size, const char *file, size_t line)
{
	void	*ptr;

	ptr = malloc(size);
	if (ptr != NULL)
		lh_add(ptr, size, file, line);
	return (ptr);
}

void	lh_free(void *mem_ref)
{
	lh_remove(mem_ref);
	free(mem_ref);
}

void	print_g_head(void)
{
	t_lh_block	*tmp;

	tmp = g_head;
	while (tmp)
	{
		printf("address: %p\n", tmp->address);
		printf("size: %zu\n", tmp->size);
		printf("file: %s\n", tmp->file);
		printf("line: %zu\n", tmp->line);
		tmp = tmp->next;
	}
}

void	leak_report(void)
{
	t_lh_block	*tmp;
	FILE	*log;
	size_t	leaks_size;
	size_t	allocated_blocks;

	leaks_size = 0;
	allocated_blocks = 0;
	if ((log = fopen(OUTPUT_FILE_NAME, "w")))
	{
		tmp = g_head;
		while (tmp)
		{
			allocated_blocks++;
			leaks_size += tmp->size;
			tmp = tmp->next;
		}
		fprintf(log, "┌─────────┤ LEAKS LOG ├─────────┐\n");
		fprintf(log, "│ Total leaks found: %10zu │\n", allocated_blocks);
		fprintf(log, "│ Total leaks size: %11zu │\n", leaks_size);
		fprintf(log, "├───────────────────────────────┤\n");
		tmp = g_head;
		if (tmp)
		{
			while (tmp)
			{
				fprintf(log, "│ address: %20p │\n", tmp->address);
				fprintf(log, "│ size (bytes): %15zu │\n", tmp->size);
				fprintf(log, "│ file: %23s │\n", tmp->file);
				fprintf(log, "│ line: %23zu │\n", tmp->line);
				if (tmp->next)
					fprintf(log, "├───────────────────────────────┤\n");
				tmp = tmp->next;
			}
		}
		else
			fprintf(log, "│   your code is leaks free!!   │\n");
		fprintf(log, "└───────────────────────────────┘\n");
		fclose(log);
	}
}
