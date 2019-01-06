//
// Created by hugom on 11/20/2018.
//

#ifndef FT_REGEX_TYPES_H
# define FT_REGEX_TYPES_H

typedef struct s_regex_code	t_regex_code;

typedef enum				e_regex_error
{
	re_ok = 0,
	re_invalid_param,
	re_out_of_memory,
	re_code_too_long,
	re_invalid_character,
	re_dangling_quantifier,
	re_missing_group_ender,
	re_dangling_group_ender
}							t_regex_error;

typedef enum				e_regex_flags
{
	re_none = 0
}							t_regex_flags;

typedef enum				e_regex_type
{
	re_undefined = 0,
	re_string,
	re_set,
	re_group,
	re_anchor,
	re_reference
}							t_regex_type;

typedef struct				s_regex_string
{
	size_t					len;
	char					ptr[0];
}							t_regex_string;

typedef struct				s_regex_set
{
	uint64_t				bol[2];
}							t_regex_set;

# if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#  define RSL 0
#  define RSH 1
# else
#  define RSL 1
#  define RSH 0
# endif

typedef enum				e_regex_group_flags
{
	re_normal = 0x0,
	re_non_holding = 0x1,
	re_look_ahead = 0x2,
	re_look_behind = 0x4,
	re_negative = 0x8,
	re_main_group = 0x10
}							t_regex_group_flags;

typedef struct				s_regex_branch
{
	size_t					min_len; //debug
	size_t					max_len; //debug
	t_regex_code			*code;
	t_regex_code			*last; //compilation only
}							t_regex_branch;

typedef struct				s_regex_group
{
	int						index;
	t_regex_string			*label;
	t_regex_group_flags		flags;
	size_t					nb_branches;
	t_regex_branch			*branches; //all branches are NULL terminated, the exact length unknown but are ordered at compilation time, the ones with the lowest operations first
}							t_regex_group;

typedef enum				e_regex_anchor
{
	re_start_of_string = 1,
	re_end_of_string = 2,
	re_word_boundary = 3,
	re_not_word_boudary = 4,
	re_start_of_line = 5,
	re_end_of_line = 6

}							t_regex_anchor;

union						u_regex_code
{
	t_regex_string			*string;
	t_regex_set				set;
	t_regex_group			group;
	t_regex_anchor			anchor;
};

typedef struct				s_regex_quantifier
{
	size_t					min;
	size_t					max;
	int						non_greedy;
}							t_regex_quantifier;

struct						s_regex_code
{
	t_regex_type			type;
	t_regex_code			*prev; //helper for compilation
	t_regex_code			*next; //if NULL, end of group
	t_regex_code			*parent; //reference to containing group, might not be usefull in full recursive
	t_regex_quantifier		quantifier;
	union u_regex_code		data;
};

/* example compiled regex for "Hello World!"
** test = {
** 	code_length = 2;
** 	code = {
** 		{
** 			type = re_group,
** 			next = NULL,
** 			parent = NULL,
** 			quantifier = {1, 1, 0},
** 			data = {group = {
** 				index = 0,
** 				label = NULL,
** 				nb_branches = 1,
** 				branches = {
** 					code = &code[1]
** 					min_len = 1
** 					max_len = 1
** 			}}}
** 		},
** 		{
** 			type = re_string,
** 			next = NULL,
** 			parent = &code[0],
** 			quantifier = {1, 1, 0},
** 			data = {string={
** 				len = 12,
** 				ptr = "Hello World!"
** 			}}
** 		}
** 	}
** }
*/

typedef struct				s_regex_compile_env
{
	size_t					*nb_groups;
	size_t					*nb_ncg;
	t_regex_code			*parent;
	t_regex_code			*out;
}							t_regex_compile_env;

typedef struct				s_regex
{
	t_regex_code			*code;
	size_t					nb_groups;
	size_t					nb_non_catching_groups;
	t_regex_error			error;
}							t_regex;

typedef struct				s_regex_match
{
	size_t					start;
	size_t					end;
}							t_regex_match;

#endif
