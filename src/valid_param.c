#include <stdlib.h>
#include <stdint.h>
#include "../inc/ft_regex_defines.h"
#include "../inc/ft_regex_types.h"
/*
** must always be inline since it uses the stack of the calling function to store temporary next and error
*/

__attribute__((always_inline)) inline int	valid_param(char **src,
														char ***next,
														t_regex_error **error)
{
	if (*error == NULL)
		*error = (t_regex_error[1]){re_ok};
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
