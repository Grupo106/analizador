#!/usr/bin/env bash
TEST_PATH=bin/tests
TEST_SRC=tests
SRC=src
CC_FLAGS="-fprofile-arcs -ftest-coverage -g -Wall"

mkdir -p $TEST_PATH
gcc $CC_FLAGS -o $TEST_PATH/test_analizador \
    $TEST_SRC/test_analizador.c $SRC/analizador.c

$TEST_PATH/test_analizador
