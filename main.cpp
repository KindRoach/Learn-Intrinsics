#include <immintrin.h>

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

int vector_dot(const std::vector<int> &a, const std::vector<int> &b) {
    assert(a.size() == b.size());

    int sum = 0;
    for (size_t i = 0; i < a.size(); i++) {
        sum += a[i] * b[i];
    }

    return sum;
}


int main(int argc, char *argv[]) {
    int n{1024};
    if (argc > 1) {
        n = std::stoi(argv[1]);
    }

    size_t loop{100000000};
    if (argc > 2) {
        loop = std::stoi(argv[2]);
    }

    std::vector a(n, 2);
    std::vector b(n, 3);

    int sum = measure_avg_time(loop, vector_dot, a, b);

    std::cout << "sum=" << sum << std::endl;
}
