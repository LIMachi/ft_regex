#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../inc/ft_regex_defines.h"
#include "../inc/ft_regex_types.h"
#include "../inc/ft_regex_functions.h"

//TODO finish: \w \W \s \S \d \D outside set
//TODO obligatory: ^ $ | ( (?: )
//TODO facultative: (?= (?! (?<= (?<! (?N

static inline int	extract_string(char *str,
									char **next,
									t_regex *holder)
{
	int				error;
	t_regex_code	*prev;
	t_regex_code	*tmp;

	prev = &holder->code[holder->code_length];
	if (holder->code[holder->code_length].type != re_undefined)
		if (++holder->code_length >= FT_REGEX_MAXIMUM_CODE_LENGTH)
			return (re_code_too_long);
	tmp = &holder->code[holder->code_length];
	if (tmp != prev)
		prev->next = tmp;
	*tmp = (t_regex_code){.type = re_string, .next = NULL,
						.parent = prev != tmp ? prev->parent : NULL,
						.quantifier = {.min = 1, .max = 1},
						.data = {.string = string(str, next, &error)}};
	return (error);
}

static inline int	extract_special_1(char *str,
									char **next,
									t_regex *holder,
									t_regex_code *tmp)
{
	int	error;

	if (strchr(FT_REGEX_SETS_STARTERS, *str) != NULL)
	{
		tmp->type = re_set;
		tmp->data.set = set(str, next, &error);
		return (error);
	}
	return (re_invalid_param);
}

static inline int	extract_special(char *str,
									char **next,
									t_regex *holder)
{
	int				error;
	t_regex_code	*prev;
	t_regex_code	*tmp;

	if (strchr(FT_REGEX_QUANTIFIERS_STARTERS, *str) != NULL)
	{
		if (holder->code[holder->code_length].type == re_undefined)
			return (re_dangling_quantifier);
		holder->code[holder->code_length++].quantifier = quantifier(str, next, &error);
		return (error);
	}
	prev = &holder->code[holder->code_length];
	if (holder->code[holder->code_length].type != re_undefined)
		if (++holder->code_length >= FT_REGEX_MAXIMUM_CODE_LENGTH)
			return (re_code_too_long);
	tmp = &holder->code[holder->code_length];
	if (tmp != prev)
		prev->next = tmp;
	*tmp = (t_regex_code){.type = re_undefined, .next = NULL,
			.parent = prev != tmp ? prev->parent : NULL,
			.quantifier = {.min = 1, .max = 1},
			.data = {.string = NULL}};
	return (extract_special_1(str, next, holder, tmp));
}

t_regex_error		ft_regex_compile(char *expression,
									t_regex *out,
									t_regex_flags flags)
{
	size_t			i;
	t_regex_error	error;

	if (expression == NULL || out == NULL)
		return (re_invalid_param);
	if (out->code_length == 0)
		out->code_length == FT_REGEX_MAXIMUM_CODE_LENGTH;
	i = -1;
	while (++i < out->code_length && i < FT_REGEX_MAXIMUM_CODE_LENGTH)
		out->code[i].type = re_undefined;
	out->code_length = 0;
	while (*expression != '\0')
		if (strchr(FT_REGEX_ALL_STARTERS, *expression) != NULL)
		{
			if ((error = extract_special(expression, &expression, out)) != re_ok)
				return (error);
		}
		else if (strchr(FT_REGEX_ALL_ENDERS, *expression) != NULL)
			return (re_invalid_character);
		else
		{
			if ((error = extract_string(expression, &expression, out)) != re_ok)
				return (error);
		}
	if (out->code[out->code_length].type != re_undefined)
		++out->code_length;
	return (re_ok);
}
