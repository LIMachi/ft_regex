//
// Created by hugom on 11/19/2018.
//

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../inc/ft_regex_defines.h"
#include "../inc/ft_regex_types.h"
#include "../inc/ft_regex_functions.h"

#include <stdio.h>
#include <ctype.h>

void	ft_regex_debug_set(t_regex_set set, int depth, const char *big_space)
{
	size_t	i;
	int		b;

	printf("%.*s0x%016lX%016lX = ", depth * 2, big_space,
		(uint64_t)(set >> 64), (uint64_t)(set & -1ull));
	i = -1;
	b = 0;
	printf("[");
	while (++i < 128)
		if (set & (((__uint128_t)1) << i))
		{
			if (b)
				printf(", ");
			if (!iscntrl(i))
				printf("'%c'", (int)i);
			else
				printf("'\\x%02X'", (unsigned int)i);
			b = 1;
		}
	printf("]\n");
}

void	rec_count(t_regex_code *regex, size_t *min, size_t *max, size_t *total)
{
	size_t	i;
	size_t	lmin;
	size_t	lmax;

	if (regex == NULL || regex->type == re_undefined)
		return ;
	++*total;
	rec_count(regex->next, min, max, total);
	if (regex->type != re_group)
		return ;
	i = -1;
	lmin = -1;
	lmax = 0;
	while (++i < regex->data.group.nb_branches)
	{
		rec_count(regex->data.group.branches[i].code,
				&regex->data.group.branches[i].min_len,
				&regex->data.group.branches[i].max_len, total);
		if (regex->data.group.branches[i].min_len > lmin)
			lmin = regex->data.group.branches[i].min_len;
		if (regex->data.group.branches[i].max_len < lmax)
			lmax = regex->data.group.branches[i].max_len;
	}
	*min += lmin;
	*max += lmax;
}

void	rec_print(t_regex_code *regex, int depth, const char *big_space)
{
	size_t			i;
	t_regex_group	*group;
	const char		*type_names[4] = {"undefined (ERROR)", "string", "set",
									"group"};

	if (regex == NULL)
		return ;
	printf("%1$.*2$stype: %3$s\n%1$.*2$sprev: %4$p\n%1$.*2$snext: %5$p\n%1$.*2"
		"$sparent: %6$p\n%1$.*2$squantifier: [%7$zu, %8$zu, %9$sgreedy]\n",
		big_space, depth * 2, type_names[regex->type], regex->prev,
		regex->next, regex->parent, regex->quantifier.min,
		regex->quantifier.max, regex->quantifier.non_greedy ? "non " : "");
	if (regex->type == re_string)
		printf("%1$.*2$slength: %3$d\n%1$.*2$s'%4$.*3$s'\n", big_space,
			depth * 2, (int)regex->data.string->len, regex->data.string->ptr);
	else if (regex->type == re_set)
		ft_regex_debug_set(regex->data.set, depth, big_space);
	else if (regex->type == re_group)
	{
		group = &regex->data.group;
		printf("%1$.*2$sindex: %3$d\n%1$.*2$slabel: '%4$s'\n%1$.*2$sflags: 0x%"
			"5$X\n%1$.*2$snb_branches: %6$zu\n", big_space, depth * 2,
			group->index, group->label != NULL ? group->label->ptr : "(nil)",
			group->flags, group->nb_branches);
		i = -1;
		++depth;
		while (++i < group->nb_branches)
		{
			printf("%1$.*2$smin_len: %3$zu\n%1$.*2$smax_len: %4$zu\n", big_space,
				depth * 2, group->branches[i].min_len, group->branches[i].max_len);
			rec_print(group->branches[i].code, depth, big_space);
		}
	}
}

int	ft_regex_debug(t_regex_code *regex)
{
	size_t			total;
	size_t			min;
	size_t			max;
	const char		big_space[] = "                                           "

	"                                                                         "
	"                                                                         "
	"                                                                         "
	"                                                                        ";
	if (regex->type == re_group && regex->data.group.flags & re_main_group)
	{
		total = 0;
		min = 0;
		max = 0;
		rec_count(regex, &min, &max, &total);
		printf("code length: %zu - %zu (wcs: %zu)\n", min, max, total);
	}
	rec_print(regex, 0, big_space);
	return (0);
}
