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