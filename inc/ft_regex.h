#ifndef FT_REGEX_H
# define FT_REGEX_H

# include <stdlib.h>
# include <stdint.h>
# include "ft_regex_defines.h"
# include "ft_regex_types.h"

t_regex			ft_regex_compile(char *expression,
								t_regex_flags flags,
								t_regex_error *error);

int				ft_regex_find(char *str,
							t_regex *regex,
							t_regex_match *matchs,
							int nb_matchs);

int				ft_regex_debug(t_regex_code *regex,
								t_regex_error error);

int				ft_regex_free(t_regex_code *regex);

#endif
