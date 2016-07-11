#!/usr/bin/env bash
TEST_PATH=bin/tests
TEST_SRC=tests
SRC=src
CC_FLAGS="-fopenmp -fprofile-arcs -ftest-coverage -g -Wall -std=c99"

mkdir -p $TEST_PATH
gcc $CC_FLAGS -o $TEST_PATH/test_analizador \
    $TEST_SRC/test_analizador.c $SRC/analizador.c

if [ $? -eq 0 ]
then
    $TEST_PATH/test_analizador
else
    exit 1
fi
