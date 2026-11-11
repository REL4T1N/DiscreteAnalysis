#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

const int EOF_CODE = 26;

std::pair<std::unordered_map<std::string, int>, int> encodeDictionaryKeys(const std::string& alphabet) {
    std::unordered_map<std::string, int> dictionary;
    int code = 0;
    for (char symbol : alphabet) {
        dictionary[std::string(1, symbol)] = code;
        ++code;
    }
    return {dictionary, ++code}; // т.к. 26 занято eof
}

std::pair<std::unordered_map<int, std::string>, int> decodeDictionaryKeys(const std::string& alphabet) {
    std::unordered_map<int, std::string> dictionary;
    int code = 0;
    for (code; code < alphabet.size(); ++code) {
        dictionary[code]=std::string(1, alphabet[code]);
    } 
    return {dictionary, ++code}; // т.к. 26 занято eof
} 

void lzwCompress(const std::string& encodeString, const std::string& alphabet) {
    auto [dictionary, nextCode] = encodeDictionaryKeys(alphabet);
    std::vector<int> result;
    std::string currentString;
    for (char symbol : encodeString) {
        std::string candidateString = currentString + symbol;
        if (dictionary.contains(candidateString)) {
            currentString = candidateString;
        } else {
            result.emplace_back(dictionary[currentString]);
            dictionary[candidateString] = nextCode;
            ++nextCode;
            currentString = std::string(1, symbol);
        }
    }
    result.emplace_back(dictionary[currentString]);
    result.emplace_back(EOF_CODE);
    
    for (int number : result) {
        std::cout << number << " ";
    }
    std::cout << "\n";
}

void lzwDecompress(std::vector<int> decodeArray, const std::string& alphabet) {
    if (decodeArray.empty() || decodeArray[0] == EOF_CODE) {
        std::cout << '\n';
        return;
    }
    auto [dictionary, nextCode] = decodeDictionaryKeys(alphabet);
    std::string currentString = dictionary[decodeArray[0]];
    std::string result = currentString;

    for (std::size_t i = 1; i < decodeArray.size(); ++i) {
        int code = decodeArray[i];

        if (code == EOF_CODE) {
            break;
        }
        std::string newString;
        if (dictionary.contains(code)) {
            newString = dictionary[code];
        } else {
            newString = currentString + std::string(1, currentString[0]);
        }

        std::string candidateString = currentString + std::string(1, newString[0]);
        dictionary[nextCode] = candidateString;
        ++nextCode;

        currentString = newString;
        result += currentString;
    }

    std::cout << result << '\n';
}

int main() {
    std::string action;
    std::string alphabet = "abcdefghijklmnopqrstuvwxyz"; // + EOF нужен
    std::cin >> action;  

    if (action == "compress") {
        std::string encodeString;
        std::cin >> encodeString;
        lzwCompress(encodeString, alphabet);
        return 0;
    } else if (action == "decompress") {
        std::vector<int> decodeArray;
        int x;
        while (std::cin >> x) {
            decodeArray.emplace_back(x);
            if (x == EOF_CODE) {
                break;
            }
        }

        lzwDecompress(decodeArray, alphabet);
        return 0;
    }
}
