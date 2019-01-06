//
// Created by hugom outn 11/24/2018.
//

#include <stdlib.h>
#include <stdint.h>

#include "../inc/ft_regex_defines.h"
#include "../inc/ft_regex_types.h"

t_regex_code	*new_code(t_regex_code *parent,
							t_regex_error *error,
							t_regex_type type)
{
	t_regex_code	*out;
	t_regex_branch	*branch;

	if ((out = malloc(sizeof(t_regex_code))) == NULL
			&& (*error = re_out_of_memory))
		return (NULL);
	*out = (t_regex_code){.type = type, .next = NULL, .prev = NULL,
					 .parent = parent, .quantifier = {1, 1, 0}};
	if (parent != NULL && parent->type == re_group)
	{
		branch = &parent->data.group.branches[
			parent->data.group.nb_branches - 1];
		out->prev = branch->last;
		if (branch->code == NULL)
		{
			branch->code = out;
			branch->last = out;
		}
		else
		{
			branch->last->next = out;
			branch->last = out;
		}
	}
	return (out);
}

