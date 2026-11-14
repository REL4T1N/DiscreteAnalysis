#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

struct Result {
    long long cost;
    std::vector<std::string> operations;
};

Result solveDynamic(int n) {
    const long long INF = 1'000'000'000'000'000LL;
    std::vector<long long> dp(n + 1, INF);
    dp[n] = 0;

    for (int i = n; i > 1; --i) {
        const long long cost = dp[i] + i;

        dp[i - 1] = std::min(dp[i - 1], cost);

        if (i % 2 == 0) {
            dp[i / 2] = std::min(dp[i / 2], cost);
        }

        if (i % 3 == 0) {
            dp[i / 3] = std::min(dp[i / 3], cost);
        }
    }

    std::vector<std::string> operations;

    int x = 1;
    while (x < n) {
        if (dp[x] == dp[x + 1] + (x + 1)) {
            operations.emplace_back("-1");
            ++x;
        } else if (2 * x <= n && dp[x] == dp[2 * x] + 2 * x) {
            operations.emplace_back("/2");
            x *= 2;
        } else if (3 * x <= n && dp[x] == dp[3 * x] + 3 * x) {
            operations.emplace_back("/3");
            x *= 3;
        }
    }

    std::reverse(operations.begin(), operations.end());

    return {dp[1], std::move(operations)};
}

int main() {
    int n;
    std::cin >> n;
    const auto result = solveDynamic(n);
    const auto& operations = result.operations;
    std::cout << result.cost << '\n';

    for (std::size_t i = 0; i < operations.size(); ++i) {
        if (i > 0) {
            std::cout << ' ';
        }
        std::cout << operations[i];
    }
    std::cout << '\n';

    return 0;
}
