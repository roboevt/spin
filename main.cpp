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

void coreNumberTest(int maxThreads = 0, const std::chrono::seconds duration = std::chrono::seconds(10)) {
    using std::cout, std::endl; 
    using namespace std::chrono_literals;

    maxThreads ? : maxThreads = std::thread::hardware_concurrency();

    Spinner spinner;
    Sum sum;
    SumByte sumByte;
    Float f;

    cout << "Add" << endl;
    Stats addStats = spinner.spinThreads(maxThreads, duration);
    cout << "Sum" << endl;
    Stats sumStats = sum.spinThreads(maxThreads, duration);
    cout << "SumByte" << endl;
    Stats sumByteStats = sumByte.spinThreads(maxThreads, duration);
    cout << "Float" << endl;
    Stats floatStats = f.spinThreads(maxThreads, duration);

    cout << "Cores\tAdd\tSum\tBytes\tFloat\t(GOPS)" << endl;
    cout << maxThreads << "\t" << addStats.totalRate() << "\t" << sumStats.totalRate() << "\t"
            << sumByteStats.totalRate() << "\t" << floatStats.totalRate() << endl;
}

auto main() -> int {
    using namespace std::chrono_literals;
    std::cout << std::fixed << std::setprecision(2);

    int maxThreads = std::thread::hardware_concurrency();
    
    // incrementalCoreTest(maxThreads);
   
    coreNumberTest(maxThreads, 10s);

    // sumByte.spinThreads(24, 1s);

    return 0;
}