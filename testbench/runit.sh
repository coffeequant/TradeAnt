#!/bin/bash
gcc -Wall -I/usr/local/include -c dates.c volatility.c

gcc -L/usr/local/lib volatility.o dates.o -lgsl -lgslcblas -lm

./a.out

