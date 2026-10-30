#include "leak_hunter.h"

int	main(void)
{
	char	*s;
	int		*i;

	atexit(leak_report);
	s = malloc(10);
	i = calloc(20, sizeof(int));
	i = realloc(i, 40 * sizeof(int));
	free(s);
	free(i);
	free(NULL);
	return (0);
}
