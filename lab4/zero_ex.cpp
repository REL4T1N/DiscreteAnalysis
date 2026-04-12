#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

std::vector<int> zFunction(const std::string& s) {
    int n = s.size();
    std::vector<int> z(n, 0);
    int left = 0, right = 0;

    for (int i = 1; i < n; ++i) {
        if (i <= right) {
            z[i] = std::min(right - i + 1, z[i - left]);
        }

        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
            ++z[i];
        }

        if (i + z[i] - 1 > right) {
            left = i;
            right = i + z[i] - 1;
        }
    }
    return z;
}

int main() {
    std::string pattern, text;
    std::cin >> text >> pattern;

    // тут крч объединить patter и text в одну строку с разделителем #
    std::string s = pattern + "#" + text;
    int pLength = pattern.size();
    std::vector<int> res = zFunction(s);
    for (int i = 0; i < s.size(); ++i) {
        if (res[i] == pLength) {
            std::cout << i - pLength - 1 << "\n";
        }
    }
    // std::cout << "\n";
}

