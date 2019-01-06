#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../inc/ft_regex_defines.h"
#include "../inc/ft_regex_types.h"
#include "../inc/ft_regex_functions.h"

t_regex		ft_regex_compile(char *expression,
							t_regex_flags flags,
							t_regex_error *error)
{
	t_regex_compile_env	env;
	t_regex				reg;

	(void)flags;
	(void)error;
	reg.nb_groups = 0;
	reg.nb_non_catching_groups = 0;
	reg.error = re_ok;
	env = (t_regex_compile_env){.parent = NULL, .out = NULL,
		.nb_groups = &reg.nb_groups, .nb_ncg = &reg.nb_non_catching_groups};
	reg.code = group(expression, NULL, &reg.error, env);
	if (error != NULL)
		*error = reg.error;
	return (reg);
}
