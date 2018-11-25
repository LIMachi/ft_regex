//
// Created by hugom on 11/25/2018.
//

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../inc/ft_regex_defines.h"
#include "../inc/ft_regex_types.h"
#include "../inc/ft_regex_functions.h"

int				ft_regex_free(t_regex_code *regex)
{
	t_regex_group	*group;
	size_t			i;

	if (regex == NULL)
		return (0);
	if (regex->type == re_string)
		free(regex->data.string);
	else if (regex->type == re_group)
	{
		group = &regex->data.group;
		if (group->label != NULL)
			free(group->label);
		i = -1;
		while (++i < group->nb_branches)
			ft_regex_free(group->branches[i].code);
		free(group->branches);
	}
	ft_regex_free(regex->next);
	free(regex);
	return (0);
}
