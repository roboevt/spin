// g++ -Wall -Wextra -std=c++20  -O3 -lpthread -march=native -o spin main.cpp spinner.cpp

#include <iomanip>
#include <iostream>
#include <thread>

#include "spinner.h"


auto main() -> int {
    using std::cout, std::endl; 
    using namespace std::chrono_literals;
    
    cout << std::fixed << std::setprecision(2);

    int maxThreads = std::thread::hardware_concurrency();
    Spinner spinner;
    Sum sum;
    SumByte sumByte;
    Float f;

    cout << "Cores\tAdd\tSum\tBytes\tFloat\t(GOPS)" << endl;
    for (int i = 0; i < maxThreads; ++i) {
        Stats addStats = spinner.spinThreads(i + 1, .2s);
        Stats sumStats = sum.spinThreads(i + 1, .2s);
        Stats sumByteStats = sumByte.spinThreads(i + 1, .2s);
        Stats floatStats = f.spinThreads(i + 1, .2s);
        cout << i + 1 << "\t" << addStats.totalRate() << "\t" << sumStats.totalRate() << "\t"
             << sumByteStats.totalRate() << "\t" << floatStats.totalRate() << endl;
    }

    return 0;
}