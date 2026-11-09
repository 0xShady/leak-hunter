#define LEAK_HUNTER_IMPLEMENTATION
#include "leak_hunter.h"

char	*greet(const char *name)
{
	char	*msg;

	msg = malloc(strlen(name) + 7);
	strcpy(msg, "hello ");
	strcat(msg, name);
	return (msg);
}

int	main(void)
{
	char	*msg;
	char	*copy;
	int		*numbers;

	atexit(leak_report);
	msg = greet("world");
	copy = strdup("never freed");
	numbers = calloc(4, sizeof(int));
	printf("%s\n", msg);
	free(numbers);
	free(numbers);
	(void)copy;
	return (0);
}
