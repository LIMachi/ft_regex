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

#include <stdlib.h> //strtol
#include <string.h> //strchr
#include <stdint.h> //uint64_t

#include "../inc/ft_regex_defines.h"
#include "../inc/ft_regex_types.h"
#include "../inc/ft_regex_functions.h"

#include <ctype.h> //isspace

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

static inline t_regex_quantifier			quantifier_extended(char *src,
														char **next,
														t_regex_error *error)
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

t_regex_quantifier							quantifier(char *src,
														char **next,
														t_regex_error *error)
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

t_regex_error								extract_quantifier(char *src,
														char **next,
														t_regex_error *error,
														t_regex_compile_env env)
{
	t_regex_code	*tmp;
	char			c;

	if (*error == re_ok && strchr(FT_REGEX_QUANTIFIERS_STARTERS, *src))
	{
		tmp = env.out->data.group.branches[
			env.out->data.group.nb_branches - 1].last;
		if (tmp == NULL || tmp->type == re_anchor)
			return (*error = re_dangling_quantifier);
		if (tmp->type == re_string && tmp->data.string->len > 1)
		{
			if ((tmp = new_code(env.out, error, re_set)) == NULL
					|| *error != re_ok)
				return (*error = re_out_of_memory);
			tmp->data.set = (t_regex_set){.bol = {0, 0}};
			c = tmp->prev->data.string->ptr[--tmp->prev->data.string->len];
			tmp->prev->data.string->ptr[tmp->prev->data.string->len + 1] = '\0';
			tmp->data.set.bol[c >= 64] |= 1ull << (c & 63);
		}
		tmp->quantifier = quantifier(src, next, error);
	}
	return (*error);
}