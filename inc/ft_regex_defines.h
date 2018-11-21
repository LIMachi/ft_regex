//
// Created by hugom on 11/20/2018.
//

#ifndef FT_REGEX_DEFINES_H
# define FT_REGEX_DEFINES_H

# define FT_REGEX_MAXIMUM_CODE_LENGTH 1024
# define FT_REGEX_MAXIMUM_RECURSION_DEPTH 64

# define FT_REGEX_QUANTIFIERS_STARTERS "{+*?"
# define FT_REGEX_QUANTIFIERS_ENDERS "}"

# define FT_REGEX_SETS_STARTERS "["
# define FT_REGEX_SETS_ENDERS "]"

# define FT_REGEX_GROUPS_STARTERS "("
# define FT_REGEX_GROUPS_ENDERS ")"

# define FT_REGEX_CHOICE_SEPARATORS "|"

# define FT_REGEX_ALL_STARTERS "{+*?[("
# define FT_REGEX_ALL_ENDERS "}])"

# define FT_REGEX_ALL_SPECIAL_CHAR "{+*?[(|}])"

# define FT_REGEX_ESCAPED_CHAR '\\'

/*
** the following defines are norm compliant, they declare constant data
** structures
*/

# define RSW64 {[RSL] = 0x3FFull << 48, [RSH] = 0x7FFFFFE87FFFFFEull}
# define RSW (((t_regex_set_helper){.u64 = RSW64}).set)

# define RSD64 {[RSL] = 0x3FFull << 48, [RSH] = 0ull}
# define RSD (((t_regex_set_helper){.u64 = RSD64}).set)

# define RSS64 {[RSL] = 0x100003E00ull, [RSH] = 0ull}
# define RSS (((t_regex_set_helper){.u64 = RSS64}).set)

# define SET_SPE ((t_regex_set[6]){RSS, ~RSS, RSD, ~RSD, RSW, ~RSW})

#endif
