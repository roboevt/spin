// g++ -Wall -Wextra -std=c++20  -O3 -lpthread -march=native -o spin main.cpp spinner.cpp

#include <iomanip>
#include <iostream>
#include <thread>

#include "spinner.h"

void incrementalCoreTest(const int maxThreads) {
    using std::cout, std::endl; 
    using namespace std::chrono_literals;

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
}

void maxCoreTest() {
    using std::cout, std::endl; 
    using namespace std::chrono_literals;
    
    Spinner spinner;
    Sum sum;
    SumByte sumByte;
    Float f;

    cout << "Add" << endl;
    Stats addStats = spinner.spinThreads(24, 8s);
    cout << "Sum" << endl;
    Stats sumStats = sum.spinThreads(24, 8s);
    cout << "SumByte" << endl;
    Stats sumByteStats = sumByte.spinThreads(24, 8s);
    cout << "Float" << endl;
    Stats floatStats = f.spinThreads(24, 8s);

    cout << "Cores\tAdd\tSum\tBytes\tFloat\t(GOPS)" << endl;
    cout << 24 << "\t" << addStats.totalRate() << "\t" << sumStats.totalRate() << "\t"
            << sumByteStats.totalRate() << "\t" << floatStats.totalRate() << endl;
}

auto main() -> int {
    std::cout << std::fixed << std::setprecision(2);

    int maxThreads = std::thread::hardware_concurrency();
    
    incrementalCoreTest(maxThreads);
   
    // sumByte.spinThreads(24, 1s);

    return 0;
}