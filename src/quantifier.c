/*
** {min,max}  [min, max     , greedy]           ok
** {min,}     [min, infinity, greedy]           ok
** {min}      [min, infinity, greedy]           ok
** {,max}     [0,   max     , greedy]           ok
** {,}        [0,   infinity, greedy]           ok
** {}         [0,   infinity, greedy]           ok
** *          [0,   infinity, greedy]           ok
** +          [1,   infinity, greedy]           ok
** ?          [0,   1       , greedy]           ok
** default    [1,   1       , greedy]           ok
** non greedy (extra ? after valid quantifier)  ok
** similar to strtol/strtod for above patterns
** only accepts characters in the set "*+?{}," and accepted by strtol
*/

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../inc/ft_regex_defines.h"
#include "../inc/ft_regex_types.h"
#include "../inc/ft_regex_functions.h"

#include <ctype.h> //isspace

static inline t_regex_quantifier	quantifier_extended(char *src, char **next, int *error)
{
	char				*tmp_ptr;
	size_t				tmp_val;
	t_regex_quantifier	out;

	out = (t_regex_quantifier){.min = 0, .max = (size_t)-1};
	tmp_val = strtol(src, &tmp_ptr, 0);
	if (tmp_ptr != src && !isspace(*tmp_ptr))
		out.min = tmp_val;
	if (tmp_ptr != src)
		src = tmp_ptr;
	if (*src == ',')
		++src;
	tmp_val = strtol(src, &tmp_ptr, 0);
	if (tmp_ptr != src && !isspace(*tmp_ptr))
		out.max = tmp_val;
	if (tmp_ptr != src)
		src = tmp_ptr;
	if (*src != '}')
		*error = 2;
	else
		++src;
	*next = src;
	return (out);
}

t_regex_quantifier					quantifier(char *src, char **next, int *error)
{
	t_regex_quantifier	out;

	out = (t_regex_quantifier){.min = 1, .max = 1, .non_greedy = 0};
	if (valid_param(&src, &next, &error))
		return (out);
	if (*src == '*' || *src == '+')
		out.max = (size_t)-1;
	if (*src == '*' || *src == '?')
		out.min = 0;
	if (*src == '*' || *src == '?' || *src == '+')
		*next = src + 1;
	if (*src == '{')
		out = quantifier_extended(src + 1, next, error);
	if ((out.non_greedy = (**next == '?')))
		++(*next);
	return (out);
}
