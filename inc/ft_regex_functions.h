//
// Created by hugom on 11/20/2018.
//

#ifndef FT_REGEX_FUNCTIONS_H
# define FT_REGEX_FUNCTIONS_H

t_regex_string				*string(char *src,
									char **next,
									t_regex_error *error);

/*
** prepare next and error so they can be facultative
** for all the functions of this library
*/

#include "../src/valid_param.c" //TODO: fix this shit

/*
int __attribute__((always_inline)) inline valid_param(char **src,
										char ***next,
										int **error);
*/

/*
** extract a quantifier from a string of formats:
** {min,max} [min, max]
** {min,}    [min, infinity]
** {min}     [min, infinity]
** {,max}    [0,   max]
** {,}       [0,   infinity]
** {}        [0,   infinity]
** *         [0,   infinity]
** +         [1,   infinity]
** ?         [0,   1]
** default   [1,   1]
** usage is similar to strtol/strtod, next and error are optional pointers
** if error is non-zero after calling quantifier, an error occurred.
** error == 1: src is NULL
** error == 2: missing closing }
*/

t_regex_quantifier			quantifier(char *src,
										char **next,
										t_regex_error *error);

/*
** extract a set from a string of formats:
*/

t_regex_set					set(char *src,
								char **next,
								t_regex_error *error);

/*
** extract an unescaped character from an escape sequence of the form:
** /n...
** if the value returned is negative, then a special escape sequence was found
** and need further processing
*/

int							unescape(char *src,
										int in_set,
										char **next,
										t_regex_error *error);

/*
** central function, extracts groups/references/choices and call all other
** methods to extract strings/sets/quantifiers (recursive)
** ft_regex_compile is a mere wrapper to group
*/

t_regex_code				*group(char *src,
									char **next,
									t_regex_error *error,
									t_regex_code *parent);

#endif
