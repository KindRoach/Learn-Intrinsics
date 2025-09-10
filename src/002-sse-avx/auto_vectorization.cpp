#include "common.h"

template <typename T>
T vector_dot(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    T sum = 0;
    for (size_t i = 0; i < a.size(); i++)
    {
        sum += a[i] * b[i];
    }

    return sum;
}


int main(int argc, char* argv[])
{
    int n{1024};
    if (argc > 1)
    {
        n = std::stoi(argv[1]);
    }

    size_t loop{10000000};
    if (argc > 2)
    {
        loop = std::stoi(argv[2]);
    }

    std::vector a(n, 2);
    std::vector b(n, 3);

    auto sum = measure_avg_time(loop, vector_dot<int>, a, b);

    std::cout << "sum=" << sum << std::endl;
}
