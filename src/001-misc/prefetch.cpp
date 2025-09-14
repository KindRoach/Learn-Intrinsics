#include <iostream>
#include <vector>
#include <chrono>

#include <immintrin.h>

#include "common.h"

constexpr size_t N = 100'000'000; // 100M elemenst
constexpr size_t loop = 100;
constexpr int PREFETCH_DISTANCE = 64; // tune for CPU

// Baseline sum (no prefetch)
size_t baseline_sum(const std::vector<int> &data) {
    size_t sum = 0;
    size_t size = data.size();
    for (size_t i = 0; i < size; i++) {
        sum += data[i];
    }
    return sum;
}

// Prefetched sum
size_t prefetched_sum(const std::vector<int> &data) {
    size_t sum = 0;
    size_t size = data.size();
    for (size_t i = 0; i < size; i++) {
        if (i + PREFETCH_DISTANCE < size) {
            _mm_prefetch((const char*)&data[i + PREFETCH_DISTANCE], _MM_HINT_T0);
        }
        sum += data[i];
    }
    return sum;
}

int main() {
    std::vector<int> data(N, 1);

    std::cout << "Baseline:\n";
    measure_avg_time(loop, baseline_sum, data);

    CodeTimer::reset();

    std::cout << "With Prefetch:\n";
    measure_avg_time(loop, prefetched_sum, data);
}
