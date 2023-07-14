#ifndef SPINNER_H
#define SPINNER_H

#include <chrono>
#include <latch>
#include <mutex>
#include <stop_token>
#include <vector>

struct Stats {
    uint64_t totalCount;
    std::chrono::duration<double> totalTime;
    int threads;

    double averageRate() const { return totalCount / totalTime.count() / 1e9; }
    double totalRate() const { return totalCount / (totalTime.count() / threads) / 1e9; }

    Stats(int threads) : totalCount(0), totalTime(0), threads(threads) {}
};

class Spinner {
   public:
    Stats spinThreads(int numThreads, std::chrono::duration<double> time);
    virtual uint64_t spinFunction(std::stop_token stop);
};

class Sum : public Spinner {
   public:
    Sum();

   protected:
    uint64_t spinFunction(std::stop_token stop) override;

   private:
    std::vector<int> m_v;
};

class SumByte : public Spinner {
   public:
    SumByte();

   protected:
    uint64_t spinFunction(std::stop_token stop) override;

   private:
    std::vector<int8_t> m_v;
};

class Float : public Spinner {
   public:
    Float();

   protected:
    uint64_t spinFunction(std::stop_token stop) override;

   private:
    std::vector<float> m_v;
};

#endif  // SPINNER_H