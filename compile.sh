gcc -fPIC -g -c -Wall *.c
gcc -g -shared -Wl,-export-dynamic -o libtradeant.so *.o -lc
#cp *.h /usr/local/include/tradeant
cp libtradeant.so /usr/local/lib
ldconfig
gcc -L/usr/local/lib newone.o -lgsl -lgslcblas -ltradeant -lm    
./a.out



