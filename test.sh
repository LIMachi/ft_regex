#!/usr/bin/env bash
make && gcc test.c -L. -lft_regex && valgrind ./a.out $*
