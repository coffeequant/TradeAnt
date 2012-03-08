#!/bin/bash
gcc -fPIC -g -c -Wall *.c
gcc -shared -Wl -o libtradeant.so *.o -lc
cp *.h /usr/local/include/tradeant
cp libtradeant.so /usr/local/lib
ldconfig



