//
// Created by hugom on 11/16/2018.
//

#include "inc/ft_regex.h"
#include <stdio.h>

int	main(int argc, char **argv)
{
	t_regex_code	*regex;
	t_regex_error	error;
	t_regex_group	groups[1];

	if (argc != 3)
		return (printf("%s regex text\n", argv[0]), 0);
	regex = ft_regex_compile(argv[1], 0, &error);
	ft_regex_debug(regex, error);
	//ft_regex_find(argv[2], &regex, groups, 1); //TODO: implement find
	ft_regex_free(regex); //TODO: implement free
}
