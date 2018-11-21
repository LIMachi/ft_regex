//
// Created by hugom on 11/20/2018.
//

//TODO: | ( (?: ) (?= (?! (?<= (?<! (?N

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../inc/ft_regex_defines.h"
#include "../inc/ft_regex_types.h"
#include "../inc/ft_regex_functions.h"

//DEBUG: quick implementation of reallocf
void	*reallocf(void *ptr, size_t size)
{
	void	*out;

	out = reallocf(ptr, size);
	if (out == NULL && ptr != NULL && size != 0)
		free(ptr);
	return (out);
}

static inline t_regex_flags	group_extract_flags(char *src,
												char **next,
												int *error)
{
	*next = src;
	*error = re_ok;
	if (*(*next) == '(')
	{
		if (*++(*next) == '?'){
			if (*++(*next) == ':')
				return (re_normal | re_non_holding);
			else if (*(*next) == '=')
				return (re_normal | re_look_ahead);
			else if (*(*next) == '!')
				return (re_normal | re_negative);
			else if (*(*next) == 'N')
				return (re_normal | re_reference); //TODO: extract name/id
			else if (*(*next) == '<'){
				if (*++(*next) == '=')
					return (re_normal | re_look_behind);
				else if (*(*next) == '!')
					return (re_normal | re_look_behind | re_negative);
			}
		}
		*error = re_invalid_character;
	}
	return (re_normal);
}

static t_regex_code			*group_rec(char *src,
										char **next,
										int *error,
										t_regex_code *out)
{
	*next = src;
	*error = re_ok;
	while (*error == re_ok && **next != '\0' && **next != ')')
		if (strchr(FT_REGEX_CHOICE_SEPARATORS, **next) != NULL)
			if ((out->data.group.branches = reallocf(
					out->data.group.branches, sizeof(t_regex_branch)
					* ++out->data.group.nb_branches)) == NULL)
				*error = re_out_of_memory;
			else
				out->data.group.branches[out->data.group.nb_branches] =
					(t_regex_branch){.code = NULL, .max_len = 0, .min_len = 0};
		else if (strchr(FT_REGEX_ALL_STARTERS, **next) != NULL)
			NULL; //TODO: extract any special
		else if (strchr(FT_REGEX_ALL_ENDERS, **next) != NULL)
			*error = re_invalid_character;
		else if (**next == '\\' && unescape(*next, 0, NULL, error) < 0)
			NULL; //TODO: extract special escape sequence
		else
			NULL; //TODO: extract string
	if (**next != ')' && !(out->data.group.flags & re_main_group))
		*error = re_missing_group_ender;
	if (**next == ')' && out->data.group.flags & re_main_group)
		*error = re_dangling_group_ender;
	return (out);
}

t_regex_code				*group(char *src,
									char **next,
									int *error,
									t_regex_code *parent)
{
	t_regex_code	*out;
	t_regex_group	group;

	if (valid_param(&src, &next, &error))
		return (NULL);
	if (parent != NULL && *src != '(')
		return (*error = re_invalid_character, NULL);
	if ((out = malloc(sizeof(t_regex_code))) == NULL)
		return (*error = re_out_of_memory, NULL);
	group = (t_regex_group){.index = 0, .label = NULL, .nb_branches = 1,
		.flags = parent == NULL ? re_main_group : 0, .branches = NULL};
	if (parent != NULL)
		group.flags = group_extract_flags(src, &src, error);
	if (*error != re_ok || ((group.branches = malloc(sizeof(t_regex_branch)))
		== NULL && (*error = re_out_of_memory)))
	{
		free(out);
		return (NULL);
	}
	*out = (t_regex_code){.type = re_group, .prev = NULL, .parent = parent,
			.quantifier = {1, 1, 0}, .next = NULL, .data = {.group = group}};
	out->data.group.branches[0] = (t_regex_branch){.min_len = 0, .max_len = 0,
												.code = NULL};
	return (group_rec(src, next, error, out));
}