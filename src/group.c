//
// Created by hugom on 11/20/2018.
//

//TODO: | ( (?: ) (?= (?! (?<= (?<! (?N

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../inc/ft_regex_defines.h"
#include "../inc/ft_regex_types.h"
#include "../inc/ft_regex_functions.h"

//DEBUG: quick implementation of reallocf, do not count toward the 5 functions limit per file
void	*reallocf(void *ptr, size_t size)
{
	void	*out;

	out = realloc(ptr, size);
	if (out == NULL && ptr != NULL && size != 0)
		free(ptr);
	return (out);
}

static inline t_regex_group_flags	group_extract_flags(char *src,
												char **next,
												t_regex_error *error)
{
	t_regex_group_flags	flags;

	*error = re_ok;
	flags = re_normal;
	if (*src == '(')
	{
		if (*++src == '?')
		{
			if (*++src == ':')
				flags = re_normal | re_non_holding;
			else if (*src == '=')
				flags = re_normal | re_look_ahead;
			else if (*src == '!')
				flags = re_normal | re_negative;
			else if (*src == '<')
			{
				if (*++src == '=')
					flags = re_normal | re_look_behind;
				else if (*src == '!')
					flags = re_normal | re_look_behind | re_negative;
			}
			++src;
			if (flags == re_normal)
				*error = re_invalid_character;
		}
	}
	*next = src;
	return (flags);
}

static inline t_regex_error	extract_special_escape(char *src,
													char **next,
													t_regex_error *error,
													t_regex_code *out)
{
	t_regex_code	*tmp;
	int				c;

	if (*error != re_ok)
		return (*error);
	c = -unescape(src, 0, next, error) - 1;
	if ((tmp = new_code(out, error, c < 6 ? re_set : re_anchor)) == NULL
			|| *error != re_ok)
		return (*error);
	if (c < 6)
		tmp->data.set = SET_SPE[c];
	else
		tmp->data.anchor = c - 5;
	return (*error);
}

static inline t_regex_error	extract_string(char *src,
											char **next,
											t_regex_error *error,
											t_regex_code *out)
{
	t_regex_string	*s;
	t_regex_code	*tmp;

	if (*error != re_ok)
		return (*error);
	s = string(src, next, error);
	if (*error != re_ok)
		return (*error);
	if ((tmp = new_code(out, error, re_string)) == NULL || *error != re_ok)
	{
		free(s);
		return (*error);
	}
	tmp->data.string = s;
	return (re_ok);
}

static inline t_regex_error	extract_any(char *src,
										char **next,
										t_regex_error *error,
										t_regex_code *out)
{
	t_regex_code	*tmp;
	t_regex_branch	*branch;

	if (*error != re_ok)
		return (*error);
	if (strchr(FT_REGEX_QUANTIFIERS_STARTERS, *src))
	{
		out = out->data.group.branches[out->data.group.nb_branches - 1].last;
		if (out == NULL || out->type == re_anchor)
			return (*error = re_dangling_quantifier);
		out->quantifier = quantifier(src, next, error);
	}
	else if (strchr(FT_REGEX_SETS_STARTERS, *src))
	{
		if ((tmp = new_code(out, error, re_set)) == NULL || *error != re_ok)
			return (*error);
		tmp->data.set = set(src, next, error);
		if (*error != re_ok)
		{
			free(tmp);
			return (*error);
		}
	}
	else if (strchr(FT_REGEX_GROUPS_STARTERS, *src))
	{
		tmp = group(src, next, error, out);
		if (*error != re_ok)
			return (*error);
		branch = &out->data.group.branches[
			out->data.group.nb_branches - 1];
		tmp->prev = branch->last;
		if (branch->code == NULL)
		{
			branch->code = tmp;
			branch->last = tmp;
		}
		else
		{
			branch->last->next = tmp;
			branch->last = tmp;
		}
	}
	return (re_ok);
}

static t_regex_code			*group_rec(char *src,
										char **next,
										t_regex_error *error,
										t_regex_code *out)
{
	*next = src;
	*error = re_ok;
	while (*error == re_ok && **next != '\0' && **next != ')')
	{
		if (strchr(FT_REGEX_CHOICE_SEPARATORS, **next) != NULL)
		{
			if ((out->data.group.branches = reallocf(
					out->data.group.branches, sizeof(t_regex_branch)
											  * ++out->data.group.nb_branches)) == NULL)
				*error = re_out_of_memory;
			else
				out->data.group.branches[out->data.group.nb_branches - 1] =
						(t_regex_branch) {.code = NULL, .max_len = 0, .min_len = 0};
		++*next;
		}
		else if (strchr(FT_REGEX_ALL_STARTERS, **next) != NULL)
			extract_any(*next, next, error, out);
		else if (strchr(FT_REGEX_ALL_ENDERS, **next) != NULL)
			*error = re_invalid_character;
		else if (**next == '\\' && unescape(*next, 0, NULL, error) < 0)
			extract_special_escape(*next, next, error, out);
		else
			extract_string(*next, next, error, out);
	}
	if (**next != ')' && !(out->data.group.flags & re_main_group))
		*error = re_missing_group_ender;
	if (**next == ')' && out->data.group.flags & re_main_group)
		*error = re_dangling_group_ender;
	if (*error == re_ok && !(out->data.group.flags & re_main_group))
		++*next;
	return (out);
}

t_regex_code				*group(char *src,
									char **next,
									t_regex_error *error,
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
												.code = NULL, .last = NULL};
	return (group_rec(src, next, error, out));
}
