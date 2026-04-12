#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

// 1. Z-функция
vector<int> computeZ(const vector<string>& words) {
    int n = words.size();
    vector<int> z(n, 0);
    int l = 0, r = 0;
    for (int i = 1; i < n; i++) {
        if (i <= r) z[i] = min(r - i + 1, z[i - l]);
        while (i + z[i] < n && words[z[i]] == words[i + z[i]]) z[i]++;
        if (i + z[i] - 1 > r) { l = i; r = i + z[i] - 1; }
    }
    return z;
}

// 2. Наивный алгоритм (Brute Force)
int naiveSearch(const vector<string>& text, const vector<string>& pat) {
    if (pat.empty() || text.size() < pat.size()) return 0;
    int count = 0;
    for (size_t i = 0; i <= text.size() - pat.size(); i++) {
        bool match = true;
        for (size_t j = 0; j < pat.size(); j++) {
            if (text[i + j] != pat[j]) {
                match = false;
                break;
            }
        }
        if (match) count++;
    }
    return count;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    vector<string> pat;
    vector<string> text;
    string word;
    char c;

    // Читаем первую строку (паттерн)
    while (cin.get(c) && c != '\n') {
        if (c == ' ' || c == '\t' || c == '\r') {
            if (!word.empty()) { pat.push_back(word); word.clear(); }
        } else word += c;
    }
    if (!word.empty()) { pat.push_back(word); word.clear(); }

    // Читаем остальной текст
    while (cin >> word) text.push_back(word);

    cout << "Text words: " << text.size() << "\n";
    cout << "Pattern words: " << pat.size() << "\n\n";

    // --- ТЕСТ 1: Z-функция ---
    auto start = high_resolution_clock::now();
    vector<string> combined = pat;
    combined.push_back("$#$");
    combined.insert(combined.end(), text.begin(), text.end());
    vector<int> z = computeZ(combined);
    
    int z_count = 0;
    int pLen = pat.size();
    for (size_t i = pLen + 1; i < z.size(); i++) {
        if (z[i] == pLen) z_count++;
    }
    auto stop = high_resolution_clock::now();
    double z_time = duration<double>(stop - start).count();
    cout << "Z-function time:   " << z_time << " sec (Matches: " << z_count << ")\n";

    // --- ТЕСТ 2: Наивный алгоритм ---
    start = high_resolution_clock::now();
    int naive_count = naiveSearch(text, pat);
    stop = high_resolution_clock::now();
    double naive_time = duration<double>(stop - start).count();
    cout << "Naive search time: " << naive_time << " sec (Matches: " << naive_count << ")\n";

    // --- ТЕСТ 3: std::search ---
    start = high_resolution_clock::now();
    int std_count = 0;
    auto it = text.begin();
    while ((it = std::search(it, text.end(), pat.begin(), pat.end())) != text.end()) {
        std_count++;
        it++;
    }
    stop = high_resolution_clock::now();
    double std_time = duration<double>(stop - start).count();
    cout << "std::search time:  " << std_time << " sec (Matches: " << std_count << ")\n";

    return 0;
}