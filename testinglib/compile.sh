#!/bin/bash
gcc -Wall -I/usr/local/include -c sample.c
gcc -L/usr/local/lib sample.o -ltradeant -lgsl -lgslcblas -lm -o sample.out
