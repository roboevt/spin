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

AMD 5900x (WSL)
Cores   Add     Sum     Bytes   Float   Rand    (GOPS)
1       4.73    30.06   141.16  1.69    0.67
2       8.71    54.28   240.86  3.30    1.29
3       10.67   62.41   297.44  4.79    1.92
4       13.21   88.75   393.21  6.43    2.44
5       14.79   102.07  447.39  7.71    2.94
6       17.26   119.44  524.79  9.17    3.49
7       19.40   130.82  575.88  10.61   4.04
8       21.38   154.32  649.50  12.12   4.59
9       24.33   159.63  714.11  13.55   5.10
10      24.82   168.77  806.18  15.03   5.70
11      28.07   195.05  837.48  16.44   6.26
12      30.97   205.80  908.50  17.93   6.80
13      32.73   218.54  998.21  19.35   7.37
14      34.90   234.02  1062.45 20.82   7.91
15      37.41   250.46  1094.87 22.22   8.49
16      40.15   265.51  1181.33 23.62   8.92
17      41.10   274.88  1245.65 25.04   9.51
18      43.79   296.64  1294.46 26.48   10.05
19      45.83   306.94  1374.41 27.85   10.56
20      48.47   331.75  1402.87 29.27   11.13
21      49.27   335.67  1501.25 30.64   11.63
22      50.99   346.11  1532.87 32.02   12.12
23      51.01   334.94  1461.57 32.87   12.38
24      48.45   351.02  1536.11 34.43   13.01

Apple M2 Mac Mini:
Cores   Add     Sum     Bytes   Float   Rand    (GOPS)
1       1.68    8.85    29.11   1.19    0.59
2       3.30    16.43   56.05   2.35    1.18
3       4.79    24.45   81.20   3.41    1.71
4       6.36    32.39   107.65  4.53    2.28
5       7.54    37.51   127.24  5.34    2.70
6       8.71    42.64   146.29  6.15    3.10
7       9.90    47.43   165.25  6.93    3.48
8       10.98   52.06   183.22  7.69    3.89

