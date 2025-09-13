#!/bin/bash

gcc -o test           \
    j_std_platform.c  \
    j_std_arena.c     \
    test.c     


./test
rm test
