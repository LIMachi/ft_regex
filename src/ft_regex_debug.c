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

void	ft_regex_debug_data(t_regex_code code)
{
	size_t	i;
	int		b;

	switch (code.type)
	{
		case re_string: printf("\"%s\"\n", code.data.string->ptr); break;
		case re_set:
			printf("0x%016lX%016lX = ", (uint64_t)(code.data.set >> 64), (uint64_t)(code.data.set & 0xFFFFFFFFFFFFFFFF));
			i = -1;
			b = 0;
			printf("[");
			while (++i < 128)
				if (code.data.set & (((__uint128_t)1) << i))
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
		break;
		case re_group:

		break;
		default:;
	}
}

int	ft_regex_debug(t_regex *regex)
{
	size_t			i;
	t_regex_code	code;
	char			*type_names[6] = {"undefined (ERROR)", "string", "set", "or", "group", "reference"};

	printf("code length: %zu\n", regex->code_length);
	i = -1;
	while (++i < regex->code_length)
	{
		code = regex->code[i];
		printf("type: %s\n", type_names[code.type]);
		printf("next: %p\n", code.next);
		printf("parent: %p\n", code.parent);
		printf("quantifier: [%zu, %zu, %sgreedy]\n", code.quantifier.min, code.quantifier.max, code.quantifier.non_greedy ? "non " : "");
		printf("data: %p\n", code.data.string);
		ft_regex_debug_data(code);
	}
	return (0);
}
