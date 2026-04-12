#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

inline void toLower(std::string& s) {
    for (char &c : s) {
        if (c >= 'A' && c <= 'Z') {
            c = c + ('a' - 'A');
        }
    }
}

std::vector<int> computeZ(const std::vector<std::string>& words) {
    int n = words.size();
    std::vector<int> z(n, 0);
    int l = 0, r = 0;
    for (int i = 1; i < n; i++) {
        if (i <= r) {
            z[i] = std::min(r - i + 1, z[i - l]);
        }
        while (i + z[i] < n && words[z[i]] == words[i + z[i]]) {
            z[i]++;
        }
        if (i + z[i] - 1 > r) {
            l = i;
            r = i + z[i] - 1;
        }
    }
    return z;
}

struct WordPos {
    int line;
    int wordIdx;
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::string patternLine;
    if (!std::getline(std::cin, patternLine)) return 0;

    std::vector<std::string> patternWords;
    std::stringstream ssPat(patternLine);
    std::string word;
    while (ssPat >> word) {
        toLower(word);
        patternWords.emplace_back(word);
    }

    if (patternWords.empty()) return 0;

    std::vector<std::string> textWords;
    std::vector<WordPos> textMeta;
    std::string line;
    int currentLine = 1;

    while (std::getline(std::cin, line)) {
        std::stringstream ssText(line);
        int currentWordIdx = 1;
        while (ssText >> word) {
            toLower(word);
            textWords.emplace_back(word);
            textMeta.emplace_back(WordPos{currentLine, currentWordIdx});
            currentWordIdx++;
        }
        currentLine++;
    }

    std::vector<std::string> combined;
    combined.reserve(patternWords.size() + textWords.size() + 1);
    
    for (const auto& w : patternWords) combined.emplace_back(w);
    combined.emplace_back("$#$");
    for (const auto& w : textWords) combined.emplace_back(w);

    std::vector<int> z = computeZ(combined);

    int pLen = patternWords.size();
    for (int i = pLen + 1; i < (int)z.size(); i++) {
        if (z[i] == pLen) {
            int textIdx = i - (pLen + 1);
            std::cout << textMeta[textIdx].line << ", " << textMeta[textIdx].wordIdx << "\n";
        }
    }

    return 0;
}