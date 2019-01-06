//
// Created by hugom on 11/16/2018.
//

#include "inc/ft_regex.h"
#include <stdio.h>

int	main(int argc, char **argv)
{
	t_regex			regex;
	t_regex_error	error;
	t_regex_match	*groups;

	if (argc != 3)
		return (printf("%s regex text\n", argv[0]), 0);
	regex = ft_regex_compile(argv[1], 0, &error);
	ft_regex_debug(regex.code, error);
	groups = malloc(sizeof(t_regex_match) * regex.nb_groups);
	for (size_t i = 0; i < regex.nb_groups; ++i)
		groups[i] = (t_regex_match){.start = -1, .end = -1};
	ft_regex_find(argv[2], &regex, groups, regex.nb_groups); //TODO: implement find
	ft_regex_free(regex.code);
}
