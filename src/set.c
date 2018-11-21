/*
** special cases:
** start with ^: reverted set
** end with -: set includes literal -
** start with - (after starting ^): set includes literal -
** char1-char2: all character in the interval [char1, char2]
** []     ""                                               ok
** [abc]  "abc"                                            ok
** [^abc] !"abc"                                           ok
** [^]    !"" (. but includes newlines and other specials) ok
** [a-c]  "abc"                                            ok
** [^a-c] !"abc"                                           ok
** [a-]   "a-"                                             ok
** [a^-]  "a^-"                                            ok
** [\-a]  "-a"                                             ok
** [--a]  all characters between - and a                   fail
** [a-c-e] "abc-e"                                         ok
** [-c-e] "-cde"                                           fail
** error == 1: src == NULL
** error == 2: not a set
** error == 3: missing closing ]
*/

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../inc/ft_regex_defines.h"
#include "../inc/ft_regex_types.h"
#include "../inc/ft_regex_functions.h"

#include <ctype.h> //isspace

static inline t_regex_set	set_interval(char **ptr,
										char **next,
										int *error,
										size_t *start)
{
	size_t				end;
	size_t				i;
	t_regex_set			out;

	out = 0;
	if (**ptr == ']')
		return (((t_regex_set)1) << '-');
	if ((int)(end = unescape(*ptr, 1, ptr, error)) < 0 && !*error)
		out = ((((t_regex_set)1) << '-') | SET_SPE[-end - 1]);
	else if (*start == '\0')
		out = ((((t_regex_set)1) << '-') | (((t_regex_set)1) << end));
	if (out == 0)
	{
		if (*start > end)
		{
			i = *start;
			*start = end;
			end = i;
		}
		i = *start - 1;
		while (++i <= end)
			out |= ((t_regex_set)1) << i;
	}
	*start = '\0';
	return (out);
}

static inline t_regex_set	set_internal(char *src,
										char **next,
										int *error)
{
	char 				*ptr;
	size_t				start;
	size_t				end;
	t_regex_set			out;

	ptr = src;
	out = 0;
	start = '\0';
	while (!*error && *ptr != '\0' && *ptr != ']' || (ptr > src && ptr[-1] == '\\'))
		if (*ptr == '-' && *(++ptr) != '\0')
			out |= set_interval(&ptr, next, error, &start);
		else if (*ptr == '\\' && (end = unescape(ptr, 1, &ptr, error)) | 1 && !*error)
		{
			start = (int)end < 0 ? '\0' : end;
			out |= (int)end < 0 ? SET_SPE[-end - 1] : ((t_regex_set)1) << end;
		}
		else
		{
			start = *ptr;
			out |= ((t_regex_set)1) << *ptr++;
		}
	if (*ptr != ']')
		*error = 3;
	*next = ptr + 1;
	return (out);
}

t_regex_set					set(char *src,
								char **next,
								int *error)
{
	if (valid_param(&src, &next, &error))
		return (0);
	if (*src != '[')
	{
		*error = 2;
		return (0);
	}
	++src;
	if (*src == '^')
		return (~set_internal(src + 1, next, error));
	return (set_internal(src, next, error));
}
