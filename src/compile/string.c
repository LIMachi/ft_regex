//
// Created by hugom on 11/17/2018.
//

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

static inline t_regex_string				*string_second_pass(char *src,
														char **next,
														t_regex_error *error,
														size_t tmp_size)
{
	t_regex_string	*out;
	char 			*ptr;

	if ((out = malloc(sizeof(t_regex_string) + tmp_size)) == NULL)
		return (*error = re_out_of_memory, NULL);
	ptr = src;
	out->len = 0;
	while (*src != '\0' && strchr(FT_REGEX_ALL_SPECIAL_CHAR, *src) == NULL)
		if (*src == FT_REGEX_ESCAPED_CHAR)
		{
			if ((out->ptr[out->len] = unescape(src, 0, &ptr, error)) < 0)
				break ;
			++out->len;
			src = ptr;
			if (*error != re_ok)
			{
				free(out);
				return (NULL);
			}
		}
		else
			out->ptr[out->len++] = *src++;
	out->ptr[out->len] = '\0';
	return (*next = src, out);
}

t_regex_string								*string(char *src,
													char **next,
													t_regex_error *error)
{
	char 			*ptr;
	size_t			tmp_size;

	if (valid_param(&src, &next, &error))
		return (NULL);
	tmp_size = 1;
	ptr = src;
	while (*ptr != '\0' && strchr(FT_REGEX_ALL_SPECIAL_CHAR, *ptr) == NULL)
	{
		if (*ptr == FT_REGEX_ESCAPED_CHAR)
		{
			if (unescape(ptr, 0, next, error) < 0)
				break ;
			if (*error != re_ok)
				return (NULL);
			ptr = *next;
		}
		else
			++ptr;
		++tmp_size;
	}
	return (string_second_pass(src, next, error, tmp_size));
}

t_regex_error								extract_string(char *src,
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