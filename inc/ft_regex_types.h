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
	re_group
}							t_regex_type;

typedef struct				s_regex_string
{
	size_t					len;
	char					ptr[0];
}							t_regex_string;

# if (!defined(__SIZEOF_INT128__) || __SIZEOF_INT128__ < 16)
#  error "missing type int 128"
# endif

typedef __uint128_t			t_regex_set;

typedef union				u_regex_set_helper
{
	uint64_t				u64[2];
	t_regex_set				set;
}							t_regex_set_helper;

# if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#  define RSL 0
#  define RSH 1
# else
#  define RSL 1
#  define RSH 0
# endif

/*
** A:65, Z:90, a:97, z:122, 0:48, 9:57, _:95
**   6666555555555544444444443333333333222222222211111111110000000000
**   3210987654321098765432109876543210987654321098765432109876543210
** 0b0000001111111111000000000000000000000000000000000000000000000000
** 0x3FF000000000000
**   6666555555555544444444443333333333222222222211111111110000000000
**   3210987654321098765432109876543210987654321098765432109876543210
** 0b0000011111111111111111111111111010000111111111111111111111111110
** 0x7FFFFFE87FFFFFE
** \r:13 \n:10 \t:9 \f:12 \v:11 ' ':32
**   6666555555555544444444443333333333222222222211111111110000000000
**   3210987654321098765432109876543210987654321098765432109876543210
** 0b0000000000000000000000000000000100000000000000000011111000000000
** 0x100003E00
*/

typedef enum				e_regex_group_flags
{
	re_normal = 0x0,
	re_reference = 0x1,
	re_non_holding = 0x2,
	re_look_ahead = 0x4,
	re_look_behind = 0x8,
	re_negative = 0x10,
	re_main_group = 0x20
}							t_regex_group_flags;

typedef struct				s_regex_branch
{
	size_t					min_len;
	size_t					max_len;
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

union						u_regex_code
{
	t_regex_string			*string;
	t_regex_set				set;
	t_regex_group			group;
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

typedef struct				s_regex
{
	t_regex_code			code[FT_REGEX_MAXIMUM_CODE_LENGTH];
	size_t					code_length;
}							t_regex;

typedef struct				s_regex_match
{
	size_t					start;
	size_t					end;
}							t_regex_match;

#endif
