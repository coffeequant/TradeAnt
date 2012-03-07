#!/bin/bash
gcc -Wall -I/usr/local/include -c *.c
gcc -L/usr/local/lib *.o -ltradeant -lgsl -lgslcblas -lm
