Usage: spin [options]
Options:
-n <num> Number of threads to test (default: max)
-d <num> Duration (Default 1s) For incremental test, this is the time each segment of the test will run for
-i Incremental core test (Default)
-m Max core test
-a Add test (Increments a counter)
-s Sum test (Sums values in a buffer)
-b Sum byte test (Sums byte values in a counter (great opportunity for SIMD))
-f Float test (Accumulates buffers of floats)
-l Large test (Accumulates bytes in a large (1GB) buffer - should stress memory bandwidth)
-h Show this message

Builds on Linux with G++. Tested in WSL and on RPI (ARM). Also works with modifications (Remove binding to CPUs and use gcc, not clang) on MacOs apple silicon.

g++ -Wall -Wextra -std=c++20  -O3 -lpthread -march=native -o spin main.cpp spinner.cpp


Results:
M2 Mac Mini:
Cores   Add     Sum     Bytes   Float   Rand    (GOPS)
1       1.68    8.85    29.11   1.19    0.59
2       3.30    16.43   56.05   2.35    1.18
3       4.79    24.45   81.20   3.41    1.71
4       6.36    32.39   107.65  4.53    2.28
5       7.54    37.51   127.24  5.34    2.70
6       8.71    42.64   146.29  6.15    3.10
7       9.90    47.43   165.25  6.93    3.48
8       10.98   52.06   183.22  7.69    3.89

