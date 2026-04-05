#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <cstdint>
#include <new>

// Та же самая функция нормализации для честности
inline void ToLowerCase(std::string& word) {
    for (char& c : word) {
        if (c >= 'A' && c <= 'Z') c += 32;
    }
}

int main() {
    // Отключаем синхронизацию для честности (как в нашей ЛР)
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::unordered_map<std::string, uint64_t> dict;
    std::string cmd;

    while (std::cin >> cmd) {
        try {
            if (cmd == "+") {
                std::string word;
                uint64_t val;
                std::cin >> word >> val;
                ToLowerCase(word);
                // emplace возвращает пару: итератор и флаг успешной вставки
                auto [it, inserted] = dict.emplace(word, val);
                if (inserted) std::cout << "OK\n";
                else std::cout << "Exist\n";
            } 
            else if (cmd == "-") {
                std::string word;
                std::cin >> word;
                ToLowerCase(word);
                if (dict.erase(word)) std::cout << "OK\n";
                else std::cout << "NoSuchWord\n";
            } 
            else if (cmd == "!") {
                std::string action, path;
                std::cin >> action >> path;
                if (action == "Save") {
                    std::ofstream out(path, std::ios::binary);
                    if (!out.is_open()) {
                        std::cout << "ERROR: Failed to save file\n";
                        continue;
                    }
                    size_t size = dict.size();
                    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
                    for (const auto& [k, v] : dict) {
                        uint16_t len = k.size();
                        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
                        out.write(k.c_str(), len);
                        out.write(reinterpret_cast<const char*>(&v), sizeof(v));
                    }
                    if (out.good()) std::cout << "OK\n";
                    else std::cout << "ERROR: Failed to save file\n";
                } 
                else if (action == "Load") {
                    std::ifstream in(path, std::ios::binary);
                    if (!in.is_open()) {
                        std::cout << "ERROR: Failed to load file\n";
                        continue;
                    }
                    size_t size;
                    if (!in.read(reinterpret_cast<char*>(&size), sizeof(size))) {
                        std::cout << "ERROR: Failed to load file\n";
                        continue;
                    }
                    std::unordered_map<std::string, uint64_t> temp;
                    bool ok = true;
                    for (size_t i = 0; i < size; ++i) {
                        uint16_t len;
                        if (!in.read(reinterpret_cast<char*>(&len), sizeof(len))) { ok = false; break; }
                        std::string k(len, ' ');
                        if (!in.read(&k[0], len)) { ok = false; break; }
                        uint64_t v;
                        if (!in.read(reinterpret_cast<char*>(&v), sizeof(v))) { ok = false; break; }
                        temp[k] = v;
                    }
                    if (ok) {
                        dict = std::move(temp);
                        std::cout << "OK\n";
                    } else {
                        std::cout << "ERROR: Failed to load file\n";
                    }
                }
            } 
            else {
                ToLowerCase(cmd);
                auto it = dict.find(cmd);
                if (it != dict.end()) std::cout << "OK: " << it->second << "\n";
                else std::cout << "NoSuchWord\n";
            }
        } 
        catch (const std::bad_alloc&) {
            std::cout << "ERROR: Out of memory\n";
        } 
        catch (...) {
            std::cout << "ERROR: Unknown\n";
        }
    }
    return 0;
}