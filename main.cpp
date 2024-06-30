// g++ -Wall -Wextra -std=c++20  -O3 -lpthread -march=native -o spin main.cpp spinner.cpp

#include <iomanip>
#include <iostream>
#include <thread>

#include <unistd.h>

#include "spinner.h"

void incrementalCoreTest(const int maxThreads, const std::chrono::duration<double> duration = std::chrono::duration<double>(0.2)) {
    using std::cout, std::endl; 
    using namespace std::chrono_literals;

    Spinner spinner;
    Sum sum;
    SumByte sumByte;
    Float f;
    spinner.setQuiet();
    sum.setQuiet();
    sumByte.setQuiet();
    f.setQuiet();

    cout << "Cores\tAdd\tSum\tBytes\tFloat\t(GOPS)" << endl;
    for (int i = 0; i < maxThreads; ++i) {
        Stats addStats = spinner.spinThreads(i + 1, duration);
        Stats sumStats = sum.spinThreads(i + 1, duration);
        Stats sumByteStats = sumByte.spinThreads(i + 1, duration);
        Stats floatStats = f.spinThreads(i + 1, duration);
        cout << i + 1 << "\t" << addStats.allCoreRate() << "\t" << sumStats.allCoreRate() << "\t"
             << sumByteStats.allCoreRate() << "\t" << floatStats.allCoreRate() << endl;
    }
}

void coreNumberTest(int maxThreads, const std::chrono::duration<double> duration = std::chrono::duration<double>(10)) {
    using std::cout, std::endl; 
    using namespace std::chrono_literals;

    Spinner spinner;
    Sum sum;
    SumByte sumByte;
    Float f;

    spinner.spinThreads(maxThreads, duration);
    sum.spinThreads(maxThreads, duration);
    sumByte.spinThreads(maxThreads, duration);
    f.spinThreads(maxThreads, duration);
}

int usage() {
    std::cout << "Usage: spin [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "-n <num> Number of threads to test (default: max)" << std::endl;
    std::cout << "-d <num> Duration (Default 1s) For incremental test, this is the time each segment of the test will run for" << std::endl;
    std::cout << "-i Incremental core test (Default)" << std::endl;
    std::cout << "-m Max core test" << std::endl;
    std::cout << "-a Add test" << std::endl;
    std::cout << "-s Sum test" << std::endl;
    std::cout << "-b Sum byte test" << std::endl;
    std::cout << "-f Float test" << std::endl;
    std::cout << "-h Show this message" << std::endl;
    return 1;
}

auto main(int argc, char** argv) -> int {
    using namespace std::chrono_literals;
    std::cout << std::fixed << std::setprecision(2);

    enum class TestType { MAX, INCREMENTAL, ADD, SUM, SUM_BYTE, FLOAT} testType = TestType::INCREMENTAL;

    int maxThreads = std::thread::hardware_concurrency();
    std::chrono::duration<double> duration = 1s;

    int c;

    while((c = getopt(argc, argv, "n:d:himasbf")) != -1) {
        switch(c) {
            case 'n':
                maxThreads = std::stoi(optarg);
                break;
            case 'd':
                duration = std::chrono::duration<double>(std::stof(optarg));
                break;
            case 'h':
                return usage();
            case 'i':
                testType = TestType::INCREMENTAL;
                break;
            case 'm':
                testType = TestType::MAX;
                break;
            case 'a':
                testType = TestType::ADD;
                break;
            case 's':
                testType = TestType::SUM;
                break;
            case 'b':
                testType = TestType::SUM_BYTE;
                break;
            case 'f':
                testType = TestType::FLOAT;
                break;
        }
    }
    
    switch(testType) {
        case TestType::MAX:
            coreNumberTest(maxThreads, duration);
            break;
        case TestType::INCREMENTAL:
            incrementalCoreTest(maxThreads, duration);
            break;
        case TestType::ADD: {
            Spinner spinner;
            spinner.spinThreads(maxThreads, duration);
            break; }
        case TestType::SUM: {
            Sum sum;
            sum.spinThreads(maxThreads, duration);
            break;}
        case TestType::SUM_BYTE: {
            SumByte sumByte;
            sumByte.spinThreads(maxThreads, duration);
            break; }
        case TestType::FLOAT: {
            Float f;
            f.spinThreads(maxThreads, duration);
            break; }
    }

    return 0;
}