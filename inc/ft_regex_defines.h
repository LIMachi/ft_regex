//
// Created by hugom on 11/20/2018.
//

#ifndef FT_REGEX_DEFINES_H
# define FT_REGEX_DEFINES_H

# define FT_REGEX_MAXIMUM_CODE_LENGTH 1024
# define FT_REGEX_MAXIMUM_RECURSION_DEPTH 64

# define FT_REGEX_QUANTIFIERS_STARTERS "{+*?"
# define FT_REGEX_QUANTIFIERS_ENDERS "}\0\0\0"

# define FT_REGEX_SETS_STARTERS "[."
# define FT_REGEX_SETS_ENDERS "]\0"

# define FT_REGEX_GROUPS_STARTERS "("
# define FT_REGEX_GROUPS_ENDERS ")"

# define FT_REGEX_CHOICE_SEPARATORS "|"

# define FT_REGEX_ALL_STARTERS "{[(+*?.^$"
# define FT_REGEX_ALL_ENDERS "}])\0\0\0\0\0\0"

# define FT_REGEX_ANCHORS "^$"

# define FT_REGEX_ALL_SPECIAL_CHAR "{+*?[(|}])^$."

# define FT_REGEX_ESCAPED_CHAR '\\'

/*
** the following defines are norm compliant, they declare constant data
** structures
*/

/*
** A:65, Z:90, a:97, z:122, 0:48, 9:57, _:95
**   6666555555555544444444443333333333222222222211111111110000000000
**   3210987654321098765432109876543210987654321098765432109876543210
** 0b0000001111111111000000000000000000000000000000000000000000000000
** 0x03FF000000000000 -> 0x3FF << 48
**   6666555555555544444444443333333333222222222211111111110000000000
**   3210987654321098765432109876543210987654321098765432109876543210
** 0b0000011111111111111111111111111010000111111111111111111111111110
** 0x07FFFFFE87FFFFFE
** \r:13 \n:10 \t:9 \f:12 \v:11 ' ':32
**   6666555555555544444444443333333333222222222211111111110000000000
**   3210987654321098765432109876543210987654321098765432109876543210
** 0b0000000000000000000000000000000100000000000000000011111000000000
** 0x0000000100003E00
*/

# define RSW64 {[0] = 0x3FFull << 48, [1] = 0x07FFFFFE87FFFFFEull}
# define RSW ((t_regex_set){.bol = RSW64})

# define NRSW64 {[0] = ~(0x3FFull << 48), [1] = ~0x07FFFFFE87FFFFFEull}
# define NRSW ((t_regex_set){.bol = NRSW64})

# define RSD64 {[0] = 0x3FFull << 48, [1] = 0ull}
# define RSD ((t_regex_set){.bol = RSD64})

# define NRSD64 {[0] = ~(0x3FFull << 48), [1] = ~0ull}
# define NRSD ((t_regex_set){.bol = NRSD64})

# define RSS64 {[0] = 0x0000000100003E00ull, [1] = 0ull}
# define RSS ((t_regex_set){.bol = RSS64})

# define NRSS64 {[0] = ~0x0000000100003E00ull, [1] = ~0ull}
# define NRSS ((t_regex_set){.bol = NRSS64})

# define SET_SPE ((t_regex_set[6]){RSS, NRSS, RSD, NRSD, RSW, NRSW})

# define DS64 {[0] = -1ull ^ (1ull << '\n'), [1] = -1ull}
# define DS ((t_regex_set){.bol = DS64})

# define NULLSET64 {[0] = 0, [1] = 0}
# define NULLSET ((t_regex_set){.bol = NULLSET64})

#endif
