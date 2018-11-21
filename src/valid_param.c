#include <stdlib.h>

/*
** must always be inline since it uses the stack of the calling function to store temporary next and error
*/

__attribute__((always_inline)) inline int	valid_param(char **src, char ***next, int **error)
{
	if (*error == NULL)
		*error = (int[1]){0};
	**error = 0;
	if (*src == NULL)
	{
		**error = 1;
		return (1);
	}
	if (*next == NULL)
		*next = (char*[1]){*src};
	return (0);
}
