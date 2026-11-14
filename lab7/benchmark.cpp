#include <chrono>
#include <iomanip>
#include <stdexcept>
#define main dynamicProgramMain
#include "main.cpp"
#undef main

// Recursive implementation.
Result solveRecursive(int n) {
    if (n == 1) return {0, {}};
    auto best = solveRecursive(n - 1);
    std::string op = "-1";
    if (n % 2 == 0) {
        auto candidate = solveRecursive(n / 2);
        if (candidate.cost < best.cost) {
            best = std::move(candidate);
            op = "/2";
        }
    }
    if (n % 3 == 0) {
        auto candidate = solveRecursive(n / 3);
        if (candidate.cost < best.cost) {
            best = std::move(candidate);
            op = "/3";
        }
    }
    best.cost += n;
    // Stored in reverse order until the outermost call returns.
    best.operations.push_back(op);
    return best;
}

Result recursiveWithPath(int n) {
    auto result = solveRecursive(n);
    std::reverse(result.operations.begin(), result.operations.end());
    return result;
}

int main(int argc, char** argv) {
    if (argc != 3) return 1;
    const int n = std::stoi(argv[2]);
    const auto start = std::chrono::steady_clock::now();
    const auto result = std::string(argv[1]) == "dp"
        ? solveDynamic(n) : recursiveWithPath(n);
    const double seconds = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - start).count();
    std::cout << result.cost << ' ' << std::fixed
              << std::setprecision(9) << seconds << '\n';
}
