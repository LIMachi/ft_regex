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

static inline t_regex_error	extract_any(char *src,
										char **next,
										t_regex_error *error,
										t_regex_compile_env env)
{
	t_regex_code	*tmp;
	t_regex_branch	*branch;

	if (*error != re_ok)
		return (*error);
	if (extract_quantifier(src, next, error, env) != re_ok)
		return (*error);
	if (strchr(FT_REGEX_SETS_STARTERS, *src))
	{
		if ((tmp = new_code(env.out, error, re_set)) == NULL
				|| *error != re_ok)
			return (*error);
		tmp->data.set = set(src, next, error);
		if (*error != re_ok)
		{
			free(tmp);
			return (*error);
		}
	}
	else if (strchr(FT_REGEX_ANCHORS, *src))
	{
		if ((tmp = new_code(env.out, error, re_anchor)) == NULL
				|| *error != re_ok)
			return (*error);
		tmp->data.anchor = *src == '^' ? re_start_of_string : re_end_of_string;
		*next = src + 1;
	}
	else if (strchr(FT_REGEX_GROUPS_STARTERS, *src))
	{
		env.parent = env.out;
		tmp = group(src, next, error, env);
		if (*error != re_ok)
			return (*error);
		branch = &env.out->data.group.branches[
			env.out->data.group.nb_branches - 1];
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
										t_regex_compile_env env)
{
	*next = src;
	*error = re_ok;
	while (*error == re_ok && **next != '\0' && **next != ')')
	{
		if (strchr(FT_REGEX_CHOICE_SEPARATORS, **next) != NULL)
		{
			if ((env.out->data.group.branches = reallocf(
					env.out->data.group.branches, sizeof(t_regex_branch) *
						++env.out->data.group.nb_branches)) == NULL)
				*error = re_out_of_memory;
			else
				env.out->data.group.branches[
					env.out->data.group.nb_branches - 1] =
				(t_regex_branch) {.code = NULL, .max_len = 0, .min_len = 0};
		++*next;
		}
		else if (strchr(FT_REGEX_ALL_STARTERS, **next) != NULL)
			extract_any(*next, next, error, env);
		else if (strchr(FT_REGEX_ALL_ENDERS, **next) != NULL)
			*error = re_invalid_character;
		else if (**next == '\\' && unescape(*next, 0, NULL, error) < 0)
			extract_special_escape(*next, next, error, env.out);
		else
			extract_string(*next, next, error, env.out);
	}
	if (**next != ')' && !(env.out->data.group.flags & re_main_group))
		*error = re_missing_group_ender;
	if (**next == ')' && env.out->data.group.flags & re_main_group)
		*error = re_dangling_group_ender;
	if (*error == re_ok && !(env.out->data.group.flags & re_main_group))
		++*next;
	return (env.out);
}

t_regex_code				*group(char *src,
									char **next,
									t_regex_error *error,
									t_regex_compile_env env)
{
	t_regex_group	group;

	if (valid_param(&src, &next, &error))
		return (NULL);
	if (env.parent != NULL && *src != '(')
		return (*error = re_invalid_character, NULL);
	if ((env.out = malloc(sizeof(t_regex_code))) == NULL)
		return (*error = re_out_of_memory, NULL);
	group = (t_regex_group){.index = 0, .label = NULL,
		.nb_branches = 1, .branches = NULL,
		.flags = env.parent == NULL ? re_main_group : 0};
	if (env.parent != NULL)
		group.flags = group_extract_flags(src, &src, error);
	group.index = group.flags & re_non_holding ?
		-++(*env.nb_ncg) : (*env.nb_groups)++;
	if (*error != re_ok || ((group.branches = malloc(sizeof(t_regex_branch)))
		== NULL && (*error = re_out_of_memory)))
	{
		free(env.out);
		return (NULL);
	}
	*env.out = (t_regex_code){.type = re_group, .prev = NULL, .next = NULL,
	.parent = env.parent, .quantifier = {1, 1, 0},  .data = {.group = group}};
	env.out->data.group.branches[0] = (t_regex_branch){.min_len = 0,
									.max_len = 0, .code = NULL, .last = NULL};
	return (group_rec(src, next, error, env));
}
