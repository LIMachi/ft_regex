/*
** special cases:
** start with ^: reverted set
** end with -: set includes literal -
** start with - (after starting ^): set includes literal -
** char1-char2: all character in the interval [char1, char2]
** .      !"<special \n>"                                  ok
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

static inline t_regex_set	i_sts(t_regex_set set1, t_regex_set set2)
{
	set1.bol[0] |= set2.bol[0];
	set1.bol[1] |= set2.bol[1];
	return (set1);
}

static inline t_regex_set	i_cts(t_regex_set set, int c)
{
	if (c < 64)
		set.bol[0] |= 1ull << c;
	else
		set.bol[1] |= 1ull << (c - 64);
	return (set);
}

static inline t_regex_set	set_interval(char **ptr,
										t_regex_error *error,
										size_t *start)
{
	size_t				end;
	size_t				i;
	t_regex_set			out;

	out = (t_regex_set){.bol = {0, 0}};
	if (**ptr == ']')
		return (i_cts(out, '-'));
	if ((int)(end = unescape(*ptr, 1, ptr, error)) < 0 && !*error)
		out = i_sts(i_cts(out, '-'), SET_SPE[-end - 1]);
	else if (*start == '\0')
		out = i_sts(i_cts(out, '-'), i_cts(out, end));
	if (out.bol[0] == 0 && out.bol[1] == 0)
	{
		if (*start > end)
		{
			i = *start;
			*start = end;
			end = i;
		}
		i = *start - 1;
		while (++i <= end)
			out = i_cts(out, i);
	}
	*start = '\0';
	return (out);
}

static inline t_regex_set	set_internal(char *src,
										char **next,
										t_regex_error *error)
{
	char 				*ptr;
	size_t				start;
	size_t				end;
	t_regex_set			out;

	ptr = src;
	out = (t_regex_set){.bol = {0, 0}};
	start = '\0';
	while (!*error && *ptr != '\0' && (*ptr != ']' || (ptr > src && ptr[-1] == '\\')))
		if (*ptr == '-' && *(++ptr) != '\0')
			out = i_sts(out, set_interval(&ptr, error, &start));
		else if (*ptr == '\\' && (end = unescape(ptr, 1, &ptr, error)) | 1 && !*error)
		{
			start = (int)end < 0 ? '\0' : end;
			out = (int)end < 0 ? i_sts(out, SET_SPE[-end - 1]) : i_cts(out, end);
		}
		else
		{
			start = *ptr;
			out = i_cts(out, *ptr++);
		}
	if (*ptr != ']')
		*error = 3;
	*next = ptr + 1;
	return (out);
}

t_regex_set					set(char *src,
								char **next,
								t_regex_error *error)
{
	t_regex_set	out;

	out = (t_regex_set){.bol = {0, 0}};
	if (valid_param(&src, &next, &error))
		return (out);
	if (*src == '.')
	{
		*next = src + 1;
		return (DS);
	}
	if (*src != '[')
	{
		*error = re_invalid_character;
		return (out);
	}
	++src;
	if (*src == '^')
	{
		out = set_internal(src + 1, next, error);
		out.bol[0] = ~out.bol[0];
		out.bol[1] = ~out.bol[1];
		return (out);
	}
	return (set_internal(src, next, error));
}
