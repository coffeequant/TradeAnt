#!/bin/bash
gcc -fPIC -g -c -Wall *.c
gcc -shared -Wl -o libtradeant.so *.o -lc
cp libtradeant.so /usr/local/lib
ldconfig



