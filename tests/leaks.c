#include "leak_hunter.h"

int	main(void)
{
	char	*a;
	char	*b;
	char	*c;
	char	*d;

	atexit(leak_report);
	a = malloc(10);
	b = malloc(20);
	c = strdup("four");
	d = malloc(40);
	free(d);
	free(a);
	(void)b;
	(void)c;
	return (0);
}
