include ../Makefiles/default_var.mk

NAME := libft_regex.a
#TEST_ARG := "[c-a]*(dd\w|(?:non capturing group))+\b|entire new branch" ""
TEST_ARG := "(?:ncg)" ""

include ../Makefiles/lib.mk