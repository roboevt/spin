// g++ -Wall -Wextra -std=c++20  -O3 -lpthread -march=native -o spin spin.cpp

#include <pthread.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <latch>
#include <mutex>
#include <random>
#include <stop_token>
#include <thread>
#include <vector>

using namespace std::chrono;

struct Stats {
    uint64_t totalCount;
    duration<double> totalTime;
    int threads;

    double averageRate() const { return totalCount / totalTime.count() / 1e9; }
    double totalRate() const { return totalCount / (totalTime.count() / threads) / 1e9; }

    Stats(int threads) : totalCount(0), totalTime(0), threads(threads) {}
};

static int setThreadAffinity(const int core_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

/// @brief Spin on a counter until the stop token is requested
/// @param stop Checked periodically (every million operations) to see if the thread should stop
/// @return The number of operations performed
static uint64_t spin(std::stop_token stop) {
    uint64_t counter = 0;
    // only check token every 1M iterations
    while (counter % (1 << 20) || !stop.stop_requested()) {
        counter++;
    }
    return counter;
}

/// @brief Spin on a counter and measure the rate at which it can be incremented
/// @param stop Checked periodically (every million cycles) to see if the thread should stop
/// @param m Mutex to protect the stats
/// @param latch Latch to synchronize the start of the threads
/// @param id Core id to pin the thread to
/// @param stats Total operations performaed and total time spent in this thead
static void threadFunction(std::stop_token stop, std::mutex& m, std::latch& latch, int id,
                           Stats& stats) {
    if (setThreadAffinity(id)) {
        std::cerr << "Failed to set thread affinity for thread " << id << std::endl;
        return;
    }

    uint64_t localCounter = 0;

    // Ensure all threads are ready before starting
    latch.arrive_and_wait();

    const auto t_start = high_resolution_clock::now();
    localCounter = spin(stop);
    const auto t_end = high_resolution_clock::now();

    std::lock_guard<std::mutex> lock(m);  // freed when lock goes out of scope
    stats.totalCount += localCounter;
    duration<double> localTime = t_end - t_start;
    stats.totalTime += localTime;
}

/// @brief Spin up threads and measure the rate at which they can increment a counter
/// @param numThreads The number of threads to spin up
/// @param time How long to run the threads for
/// @return The rate (GHz) at which the threads can increment the counter
static Stats spinThreads(const int numThreads, const duration<double> time) {
    std::vector<std::jthread> threads(numThreads);
    std::mutex m;
    std::latch latch{numThreads};

    Stats stats(numThreads);

    // Start all threads
    for (int i = 0; i < numThreads; ++i) {
        threads[i] = std::jthread(threadFunction, std::ref(m), std::ref(latch), i, std::ref(stats));
    }

    // Wait for desired time (actual duration will be measured per thread and averaged)
    std::this_thread::sleep_for(time);

    // Terminate all threads
    for (auto& t : threads) {
        t.request_stop();
        t.join();
    }

    return stats;
}

auto main() -> int {
    using std::cout, std::endl;
    cout << std::fixed << std::setprecision(2);

    int maxThreads = std::thread::hardware_concurrency();

    cout << "Cores\tTotal\tAvg (GHZ)" << endl;
    for (int i = 0; i < maxThreads; ++i) {
        Stats stats = spinThreads(i + 1, .2s);
        cout << i + 1 << "\t" << stats.totalRate() << "\t" << stats.averageRate() << endl;
    }
    // All cores for longer duration
    Stats stats = spinThreads(maxThreads, 2s);
    cout << "ALL"
         << "\t" << stats.totalRate() << "\t" << stats.averageRate() << endl;

    return 0;
}