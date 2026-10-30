#include "leak_hunter.h"

int	main(void)
{
	char	*a;
	char	*b;
	int		x;

	atexit(leak_report);
	a = malloc(10);
	b = malloc(30);
	free(a);
	free(a);
	free(&x);
	(void)b;
	return (0);
}
