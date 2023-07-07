// g++ -Wall -Wextra -std=c++20  -O3 -lpthread -march=native -o spin spin.cpp

#include <pthread.h>

#include <chrono>
#include <iostream>
#include <latch>
#include <mutex>
#include <stop_token>
#include <thread>
#include <vector>

using namespace std::chrono;

struct Stats {
    uint64_t totalCount;
    duration<double> totalTime;
    Stats() : totalCount(0), totalTime(0) {}
};

static int setThreadAffinity(const int core_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

/// @brief Spin on a counter and measure the rate at which it can be incremented
/// @param stop Checked periodically (every million cycles) to see if the thread should stop
/// @param m Mutex to protect the stats
/// @param latch Latch to synchronize the start of the threads
/// @param id Core id to pin the thread to
/// @param stats Total operations performaed and total time spent in this thead
static void spin(std::stop_token stop, std::mutex& m, std::latch& latch, int id, Stats& stats) {
    if (setThreadAffinity(id)) {
        std::cerr << "Failed to set thread affinity for thread " << id << std::endl;
        return;
    }
    
    uint64_t localCounter = 0;

    latch.arrive_and_wait();

    const auto t_start = high_resolution_clock::now();
    while (true) {
        ++localCounter;

        // only check token every 1M iterations
        if (localCounter % (1 << 22) == 0) {
            if (stop.stop_requested()) {
                break;
            }
        }
    }
    const auto t_end = high_resolution_clock::now();

    std::lock_guard<std::mutex> lock(m);
    stats.totalCount += localCounter;
    duration<double> localTime = t_end - t_start;
    stats.totalTime += localTime;
}

/// @brief Spin up threads and measure the rate at which they can increment a counter
/// @param numThreads The number of threads to spin up
/// @param time How long to run the threads for
/// @return The rate (GHz) at which the threads can increment the counter
static double spinThreads(const int numThreads, const duration<double> time) {
    std::vector<std::jthread> threads(numThreads);
    std::mutex m;
    std::latch latch{numThreads};

    Stats stats;

    // Start all threads
    for (int i = 0; i < numThreads; ++i) {
        threads[i] = std::jthread(spin, std::ref(m), std::ref(latch), i, std::ref(stats));
    }

    // Wait for desired time (actual duration will be measured per thread and averaged)
    std::this_thread::sleep_for(time);

    // Terminate all threads
    for (auto& t : threads) {
        t.request_stop();
        t.join();
    }

    const auto avgTime = stats.totalTime / numThreads;
    // GHz
    const double rate = stats.totalCount / avgTime.count() / 1e9;

    return rate;
}

auto main() -> int {
    using std::cout, std::endl;

    int maxThreads = std::thread::hardware_concurrency();

    cout << "Cores\tRate (GHz)" << endl;
    for (int i = 0; i < maxThreads; ++i) {
        cout << i + 1 << "\t" << spinThreads(i + 1, .2s) << endl;
    }
    // All cores for longer duration
    cout << "ALL" << "\t" << spinThreads(maxThreads, 2s) << endl;

    return 0;
}