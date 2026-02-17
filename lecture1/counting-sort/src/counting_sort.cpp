#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>

struct User {
    int id;
    int score;
    std::string name;
    std::string surname;

    User(int i, int sc, std::string n, std::string sn) : id(i), score(sc), name(move(n)), surname(move(sn)) {}
};

std::vector<std::unique_ptr<User>> read_users(const std::string& filename) {
    std::vector<std::unique_ptr<User>> users;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "ошибка открытия файла " << filename << std::endl;
        return users;
    }

    std::string line;
    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string id_str, score_str, name, surname;

        getline(ss, id_str, ',');
        getline(ss, score_str, ',');
        getline(ss, name, ',');
        getline(ss, surname, ',');

        int id = stoi(id_str);
        int score = stoi(score_str);

        users.emplace_back(std::make_unique<User>(id, score, name, surname));
    }
    return users;
}

void write_users(const std::string& filename, const std::vector<User*> &users) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "ошибка создания файла " << filename << std::endl;
        return;
    }

    for (const auto& user : users) {
        file << user->id << ',' << user->score << ',' << user->name << ',' << user->surname << '\n';
    }
}

std::vector<User*> counting_sort_by_score(const std::vector<std::unique_ptr<User>> &users) {
    if (users.empty()) return {};
    
    int min_score = users[0]->score;
    int max_score = users[0]->score;

    for (const auto& user : users) {
        if (user->score < min_score) min_score = user->score;
        if (user->score > max_score) max_score = user->score;
    }

    int range_size = max_score - min_score + 1;
    std::vector<int> count(range_size, 0);

    for (const auto& user: users) {
        int index = user->score - min_score;
        count[index]++;
    }

    for (size_t i = 1; i < count.size(); i++) {
        count[i] += count[i - 1];
    }

    std::vector<User*> sorted_users(users.size());
    for (int i = users.size() - 1; i >= 0; i--) {
        int index = users[i]->score - min_score;
        int pos = count[index] - 1;
        sorted_users[pos] = users[i].get(); // сохраняется указатель
        count[index]--;
    }

    return sorted_users;
}

int main() {
    std::string input_prefix = "../data/input/users_";
    std::string output_prefix = "../data/output/cpp_sorted_users_";
    std::string format = ".csv";

    std::vector<int> users_counts = {10, 50, 100, 1000, 100000, 1000000};

    for (int count : users_counts) {
        std::string input_file = input_prefix + std::to_string(count) + format;
        std::string output_file = output_prefix + std::to_string(count) + format;

        auto users = read_users(input_file);
        if (users.empty()) {
            std::cerr << "ошибка при чтении файла " << input_file << std::endl;
            continue;
        }

        std::cout << "прочитано " << users.size() << " пользователей" << std::endl;

        auto sorted_ptrs = counting_sort_by_score(users);

        write_users(output_file, sorted_ptrs);
        std::cout << "отсортированные данные записаны в " << output_file << std::endl;
    }
    return 0;
}