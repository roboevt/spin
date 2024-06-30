#include "spinner.h"

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
#include <algorithm>

static int setThreadAffinity(const int core_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

static inline void use(uint64_t x) { asm volatile("" : "+r"(x)); }

/// @brief Spin on a counter until the stop token is requested
/// @param stop Checked periodically (every million operations) to see if the thread should stop
/// @return The number of operations performed
uint64_t Spinner::spinFunction(std::stop_token stop) {
    uint64_t counter = 0;
    // only check token every 1M iterations
    while (counter % (1 << 22) || !stop.stop_requested()) {
        counter++;
    }
    return counter;
}

Sum::Sum() : m_v(numElements) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    std::generate(m_v.begin(), m_v.end(), [&]() { return dis(gen); });
}

// 2a80:       c5 fd fe 00             vpaddd (%rax),%ymm0,%ymm0
// 2a84:       48 83 c0 20             add    $0x20,%rax
// 2a88:       48 39 d0                cmp    %rdx,%rax
// 2a8b:       75 f3                   jne    2a80
uint64_t Sum::spinFunction(std::stop_token stop) {
    uint64_t sum = 0, i = 0;

    // only check token every 1M iterations
    while (!stop.stop_requested()) {
        sum += std::accumulate(m_v.begin(), m_v.end(), 0);
        i += m_v.size();
    }
    use(sum);
    return i;
}

SumByte::SumByte() : m_v(numElements) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    std::generate(m_v.begin(), m_v.end(), [&]() { return dis(gen); });
}

uint64_t SumByte::spinFunction(std::stop_token stop) {
    uint64_t sum = 0, i = 0;

    // only check token every 1M iterations
    while (!stop.stop_requested()) {
        sum += std::accumulate(m_v.begin(), m_v.end(), static_cast<int8_t>(0));
        i += m_v.size();
    }
    use(sum);
    return i;
}

Float::Float() : m_v(numElements) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1, 100);
    std::generate(m_v.begin(), m_v.end(), [&]() { return dis(gen); });
}

uint64_t Float::spinFunction(std::stop_token stop) {
    float sum = 0;
    uint64_t i = 0;

    // only check token every 1M iterations
    while (!stop.stop_requested()) {
        sum += std::accumulate(m_v.begin(), m_v.end(), 0.0f);
        i += m_v.size();
    }
    use(sum);
    return i;
}

/// @brief Spin on a counter and measure the rate at which it can be incremented
/// @param stop Checked periodically (every million cycles) to see if the thread should stop
/// @param m Mutex to protect the stats
/// @param latch Latch to synchronize the start of the threads
/// @param id Core id to pin the thread to
/// @param stats Total operations performaed and total time spent in this thead
static void threadFunction(std::stop_token stop, Spinner* spinner, std::mutex& m, std::latch& latch, int id,
                             Stats& stats) {
    if (setThreadAffinity(id)) {
        std::cerr << "Failed to set thread affinity for thread " << id << std::endl;
        return;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    std::vector<int8_t> v(1 << 14);  // 16kB
    for (auto& i : v) {
        i = dis(gen);
    }
    uint64_t localCounter = 0;

    // Ensure all threads are ready before starting
    latch.arrive_and_wait();

    const auto t_start = std::chrono::high_resolution_clock::now();
    // localCounter = add(stop);
    localCounter = spinner->spinFunction(stop);
    const auto t_end = std::chrono::high_resolution_clock::now();

    std::lock_guard<std::mutex> lock(m);  // freed when lock goes out of scope
    stats.totalCount += localCounter;
    std::chrono::duration<double> localTime = t_end - t_start;
    stats.totalTime += localTime;
}

/// @brief Spin up threads and measure the rate at which they can increment a counter
/// @param numThreads The number of threads to spin up
/// @param time How long to run the threads for
/// @return The rate (GHz) at which the threads can increment the counter
Stats Spinner::spinThreads(const int numThreads, const std::chrono::duration<double> time) {
    std::vector<std::jthread> threads(numThreads);
    std::mutex m;
    std::latch latch{numThreads};

    Stats stats(numThreads);

    // Start all threads
    for (int i = 0; i < numThreads; i++) {
        threads[i] = std::jthread(threadFunction, this, std::ref(m), std::ref(latch), i, std::ref(stats));
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
