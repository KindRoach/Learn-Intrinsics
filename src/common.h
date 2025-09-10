#pragma once

#include <cassert>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <functional>

class CodeTimer
{
public:
    static inline const std::string end2end{"end-to-end"};

    static void start(const std::string& name)
    {
        if (!start_times.contains(name) && !total_times.contains(name))
        {
            insertion_order.push_back(name);
        }
        start_times[name] = clock::now();
    }

    static void stop(const std::string& name)
    {
        const auto it = start_times.find(name);
        if (it == start_times.end())
        {
            std::cerr << "No start time recorded for '" << name << "'\n";
            return;
        }

        auto end = clock::now();
        auto duration = std::chrono::duration_cast<duration_t>(end - it->second);
        total_times[name] += duration;
        call_counts[name] += 1;
        start_times.erase(it);
    }

    static void print_times()
    {
        const int name_width = 20;
        const int total_width = 20;
        const int calls_width = 20;
        const int avg_width = 20;
        const int percent_width = 10;

        std::cout << std::left << std::setw(name_width) << "Name"
            << std::setw(total_width) << "Total Time"
            << std::setw(calls_width) << "Calls"
            << std::setw(avg_width) << "Avg Time"
            << std::setw(percent_width) << "Percent" << '\n';

        std::cout << std::string(name_width + total_width + calls_width + avg_width + percent_width, '-') << '\n';

        auto it = total_times.find(end2end);
        if (it == total_times.end())
        {
            std::cerr << "Missing '" << end2end << "' timer for percentage calculation.\n";
            return;
        }

        duration_t end_to_end_total = it->second;
        std::cout << std::fixed << std::setprecision(3);

        for (const auto& name : insertion_order)
        {
            auto total = total_times.at(name);
            auto count = call_counts.at(name);
            auto avg = total / count;
            double percent = 100.0 * total.count() / end_to_end_total.count();

            std::ostringstream percent_str;
            percent_str << std::fixed << std::setprecision(1) << percent << "%";

            std::cout << std::left << std::setw(name_width) << name
                << std::setw(total_width) << format_duration(total)
                << std::setw(calls_width) << count
                << std::setw(avg_width) << format_duration(avg)
                << std::setw(percent_width) << percent_str.str() << '\n';
        }

        std::cout << std::string(name_width + total_width + calls_width + avg_width + percent_width, '-') << '\n';
    }

private:
    using clock = std::chrono::high_resolution_clock;
    using duration_t = std::chrono::nanoseconds;

    static std::string format_duration(duration_t ns)
    {
        auto count = ns.count();
        if (count < 1'000)
        {
            return std::to_string(count) + " ns";
        }
        if (count < 1'000'000)
        {
            return std::to_string(count / 1'000.0) + " us";
        }
        if (count < 1'000'000'000)
        {
            return std::to_string(count / 1'000'000.0) + " ms";
        }
        return std::to_string(count / 1'000'000'000.0) + " s";
    }

    static inline std::unordered_map<std::string, size_t> call_counts;
    static inline std::vector<std::string> insertion_order;
    static inline std::unordered_map<std::string, clock::time_point> start_times;
    static inline std::unordered_map<std::string, duration_t> total_times;
};


template <typename Func, typename... Args>
auto measure_avg_time(size_t runs, Func&& f, Args&&... args)
{
    assert(runs > 0);

    using ReturnType = decltype(std::invoke(std::forward<Func>(f), std::forward<Args>(args)...));

    if constexpr (std::is_void_v<ReturnType>)
    {
        for (size_t i = 0; i < runs; ++i)
        {
            CodeTimer::start(CodeTimer::end2end);
            std::invoke(std::forward<Func>(f), std::forward<Args>(args)...);
            CodeTimer::stop(CodeTimer::end2end);
        }
        CodeTimer::print_times();
    }
    else
    {
        ReturnType result{};
        for (size_t i = 0; i < runs; ++i)
        {
            CodeTimer::start(CodeTimer::end2end);
            result = std::invoke(std::forward<Func>(f), std::forward<Args>(args)...);
            CodeTimer::stop(CodeTimer::end2end);
        }
        CodeTimer::print_times();
        return result;
    }
}
