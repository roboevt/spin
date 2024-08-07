#ifndef SPINNER_H
#define SPINNER_H

#include <chrono>
#include <latch>
#include <mutex>
#include <stop_token>
#include <vector>
#include <iostream>

struct Stats {
    uint64_t totalCount;
    std::chrono::duration<double> totalTime;
    int threads;

    double perCoreRate() const { return totalCount / totalTime.count() / 1e9; }
    double allCoreRate() const { return totalCount / (totalTime.count() / threads) / 1e9; }

    Stats(int threads) : totalCount(0), totalTime(0), threads(threads) {}
};

class Spinner {
    protected:
    Stats stats;
    bool logResults = true;
   public:
    Spinner();
    Stats spinThreads(int numThreads, std::chrono::duration<double> time);
    virtual uint64_t spinFunction(std::stop_token stop);
    virtual ~Spinner();
    void setQuiet() { logResults = false; }
    void setVerbose() { logResults = true; }
};

class Sum : public Spinner {
    constexpr static int numElements = 1 << 10; // 16KB
   public:
    Sum();
    ~Sum() override;

   protected:
    uint64_t spinFunction(std::stop_token stop) override;

   private:
    std::vector<int> m_v;
};

class SumByte : public Spinner {
    constexpr static int numElements = 1 << 14; // 16KB
   public:
    SumByte();
    ~SumByte() override;

   protected:
    uint64_t spinFunction(std::stop_token stop) override;

   private:
    std::vector<int8_t> m_v;
};

class Float : public Spinner {
    constexpr static int numElements = 1 << 10; // 16KB
   public:
    Float();
    ~Float() override;

   protected:
    uint64_t spinFunction(std::stop_token stop) override;

   private:
    std::vector<float> m_v;
};

class Large : public Spinner {
    constexpr static int64_t numElements = 1e9;  // 1GB
   public:
    Large();
    ~Large() override;

   protected:
    uint64_t spinFunction(std::stop_token stop) override;

   private:
    std::vector<uint8_t> m_v;
};

class Rand : public Spinner {
    constexpr static int64_t numElements = 1 << 20; // 1MB
   public:
    Rand();
    ~Rand() override;

   protected:
    uint64_t spinFunction(std::stop_token stop) override;

   private:
    std::vector<int> m_v;
};

#endif  // SPINNER_H