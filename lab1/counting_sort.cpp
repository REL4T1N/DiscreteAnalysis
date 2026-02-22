#include <iostream>
#include <vector>
// #include <map>
#include <algorithm>
#include <memory>
#include <string>
#include <utility>

std::vector<std::pair<int, std::string>> couting_sort(const std::vector<std::pair<int, std::string>> &data) {
    if (data.empty()) return {};

    int max_key = data[0].first;
    int min_key = data[0].first;

    for (const auto& p : data) {
        if (p.first > max_key) max_key = p.first;
        if (p.first < min_key) min_key = p.first;
    }

    int range_size = max_key - min_key + 1;
    std::vector<int> count(range_size, 0);
    for (const auto& p : data) {
        int index = p.first - min_key;
        ++count[index];
    }

    for (size_t i = 1; i < count.size(); ++i) {
        count[i] += count[i-1];
    }

    std::vector<std::pair<int, std::string>> result(data.size());
    for (int i = data.size() - 1; i >= 0; --i) {
        int index = data[i].first - min_key;
        int pos = count[index] - 1;
        result[pos] = data[i];
        --count[index];
    }
    return result;
}

int main() {
    std::ios_base::sync_with_stdio(false); 
    std::cin.tie(NULL);
    std::vector<std::pair<int, std::string>> data;
    int key;
    std::string value;

    while(std::cin >> key >> value) {
        data.emplace_back(key,value);
    }

    auto sorted_data = couting_sort(data);
    for (const auto& p : sorted_data) {
        std::cout << p.first << '\t' << p.second << '\n';
    }
    return 0;
}