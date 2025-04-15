#pragma once

#include <cassert>
#include <chrono>
#include <iostream>

template<typename Func, typename... Args>
auto measure_avg_time(size_t runs, Func &&f, Args &&... args)
    -> decltype(f(std::forward<Args>(args)...)) {
    using namespace std::chrono;

    assert(runs > 0);

    using ReturnType = decltype(f(std::forward<Args>(args)...));
    ReturnType result;

    auto start = high_resolution_clock::now();

    for (int i = 0; i < runs; ++i) {
        result = f(std::forward<Args>(args)...);
    }

    auto end = high_resolution_clock::now();
    auto total_duration = duration_cast<duration<double> >(end - start);

    std::cout << "total: " << total_duration.count() << " seconds" << std::endl;
    std::cout << "avg: " << total_duration.count() / runs << " seconds" << std::endl;

    return result;
}