#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>

#define BENCHMARK 
#include "counting_sort.cpp" 

std::string random_string(size_t length) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    std::string str(length, 0);
    for (size_t i = 0; i < length; ++i) str[i] = charset[rand() % 26];
    return str;
}

void run_test(int n) {
    std::vector<std::pair<int, std::string>> data;
    data.reserve(n);
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(0, 65535);

    for (int i = 0; i < n; ++i) {
        data.emplace_back(dis(gen), "value_" + std::to_string(i));
    }

    auto data_cs = data;
    auto start = std::chrono::high_resolution_clock::now();
    auto res_cs = counting_sort(data_cs); 
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_cs = end - start;


    auto data_std = data;
    start = std::chrono::high_resolution_clock::now();
    // std::stable_sort(data_std.begin(), data_std.end(), [](const auto& a, const auto& b) {
    //     return a.first < b.first;
    // });
    std::sort(data_std.begin(), data_std.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_std = end - start;

    std::cout << std::setw(10) << n << " | " 
              << std::setw(12) << std::fixed << std::setprecision(5) << diff_cs.count() << "s | "
              << std::setw(12) << diff_std.count() << "s | "
              << std::setw(8) << diff_std.count() / diff_cs.count() << "x" << std::endl;
}

int main() {
    std::cout << "Benchmarking: Counting Sort vs std::stable_sort\n";
    std::cout << std::string(55, '-') << std::endl;
    std::cout << std::setw(10) << "Size" << " | " << "Counting    | " << "std::stable | " << "Speedup" << std::endl;
    std::cout << std::string(55, '-') << std::endl;

    std::vector<int> sizes = {1000, 10000, 100000, 1000000, 3000000};
    for (int n : sizes) {
        run_test(n);
    }
    return 0;
}