#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../inc/ft_regex_defines.h"
#include "../inc/ft_regex_types.h"
#include "../inc/ft_regex_functions.h"

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

static inline int	unescape_internal(char *src,
										char **next,
										t_regex_error *error)
{
	static const char	escape[16] = "n\nr\rt\tv\vf\fa\ab\b0\0";
	char 				*tmp;
	int					val;

	if (*src == '\0')
	{
		*error = 4;
		return ('\0');
	}
	if (*src == 'x' || *src == 'X')
	{
		val = strtol(src + 1, &tmp, 16);
		if (tmp != src + 3 || val > 127)
		{
			*error = 3;
			return (0);
		}
		*next = src + 3;
		return (val);
	}
	if (strchr("nrtvfab0", *src) != NULL)
		return (*(strchr(escape, *src) + 1));
	return (*src);
}

int					unescape(char *src,
							int in_set,
							char **next,
							t_regex_error *error)
{
	static const char	all_spe[11] = "sSdDwWAZbB";
	static const char	in_set_spe[7] = "sSdDwW";
	char 				*tmp;

	if (valid_param(&src, &next, &error))
		return (0);
	if (*src != '\\')
	{
		*next = src + 1;
		return (*src);
	}
	*next = ++src + 1;
	if (in_set)
	{
		if ((tmp = strchr(in_set_spe, *src)) != NULL)
			return ((int)(size_t)(in_set_spe - tmp - 1));
	}
	else
	{
		if ((tmp = strchr(all_spe, *src)) != NULL)
			return ((int)(size_t)(all_spe - tmp - 1));
	}
	return (unescape_internal(src, next, error));
}
