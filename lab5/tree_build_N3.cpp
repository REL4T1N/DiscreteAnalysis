#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

// Узел суффиксного дерева
struct Node {
    map<string, Node*> edges; // Ключ - строка на ребре, Значение - следующий узел
    bool is_leaf;
    int suffix_index; // Для отладки: номер суффикса, если это лист

    Node(bool leaf = false) : is_leaf(leaf), suffix_index(-1) {}
};

class SuffixTree {
private:
    Node* root;
    string text;

    // Вспомогательная функция для прохода по дереву и применения правил Гасфилда
    void extend(int j, int i) {
        // Мы ищем конец строки S[j..i-1] (в 0-индексации)
        string beta = text.substr(j, i - j); 
        char next_char = text[i];            // Символ, который нужно добавить (S[i])
        
        Node* current = root;
        
        // 1. Спускаемся по дереву, читая строку beta
        while (!beta.empty()) {
            bool found_edge = false;
            for (auto& edge : current->edges) {
                string edge_str = edge.first;
                Node* next_node = edge.second;

                // Насколько ребро совпадает с нашим префиксом beta?
                int match_len = 0;
                while (match_len < edge_str.length() && match_len < beta.length() && edge_str[match_len] == beta[match_len]) {
                    match_len++;
                }

                if (match_len > 0) {
                    if (match_len == edge_str.length()) {
                        // Мы полностью прошли это ребро, идем в следующий узел
                        current = next_node;
                        beta = beta.substr(match_len);
                    } else {
                        // Мы остановились посередине ребра.
                        // Значит, beta закончилась именно здесь (т.к. мы точно знаем, что beta уже есть в дереве)
                        string rem_edge = edge_str.substr(match_len);
                        
                        // ПРАВИЛО 3: Символ уже есть на ребре
                        if (rem_edge[0] == next_char) {
                            return; // Ничего не делаем
                        }
                        
                        // ПРАВИЛО 2: Символа нет. Нужно разбить ребро.
                        Node* split_node = new Node(false);
                        current->edges.erase(edge_str); // Удаляем старое ребро
                        current->edges[edge_str.substr(0, match_len)] = split_node; // Ребро до разрыва
                        split_node->edges[rem_edge] = next_node; // Остаток старого ребра
                        
                        // Создаем новый лист для нового символа
                        Node* new_leaf = new Node(true);
                        new_leaf->suffix_index = j;
                        split_node->edges[string(1, next_char)] = new_leaf;
                        
                        return;
                    }
                    found_edge = true;
                    break;
                }
            }
            if (!found_edge) break; // Защита от зацикливания (не должно срабатывать)
        }

        // 2. Мы оказались в узле (current), прочитав всю beta
        if (current->is_leaf) {
            // ПРАВИЛО 1: Если мы в листе, по правилам O(N^3) мы должны просто 
            // удлинить ребро, ведущее в этот лист. 
            // Но в нашей структуре (где ребра идут ИЗ current), это эквивалентно 
            // тому, что мы находимся в узле-листе.
            // *В реальном Укконене ребра листьев растут автоматически, здесь мы делаем это явно.*
            // Для упрощения мы просто добавим символ как новое ребро, а статус листа спустим ниже.
            Node* new_leaf = new Node(true);
            new_leaf->suffix_index = j;
            current->edges[string(1, next_char)] = new_leaf;
            current->is_leaf = false; // Текущий перестал быть листом
        } else {
            // Проверяем, есть ли уже ребро, начинающееся с next_char (ПРАВИЛО 3)
            for (auto& edge : current->edges) {
                if (edge.first[0] == next_char) {
                    return; // Правило 3
                }
            }
            
            // ПРАВИЛО 2: Создаем новый лист из существующего узла
            Node* new_leaf = new Node(true);
            new_leaf->suffix_index = j;
            current->edges[string(1, next_char)] = new_leaf;
        }
    }

    // Вспомогательная функция для красивого вывода дерева
    void print(Node* node, int depth) {
        for (auto& edge : node->edges) {
            for (int i = 0; i < depth; ++i) cout << "  ";
            cout << "--(" << edge.first << ")--> ";
            if (edge.second->is_leaf) cout << "[Leaf " << edge.second->suffix_index << "]\n";
            else cout << "[Node]\n";
            print(edge.second, depth + 1);
        }
    }

public:
    SuffixTree() {
        root = new Node();
    }

    // Тот самый N^3 алгоритм
    void build_O_N3(string s) {
        text = s;
        // Фазы (i от 0 до N-1)
        for (int i = 0; i < text.length(); ++i) {
            // Расширения (j от 0 до i)
            for (int j = 0; j <= i; ++j) {
                extend(j, i);
            }
        }
    }

    void printTree() {
        cout << "Suffix Tree for: " << text << "\n";
        print(root, 0);
    }
};

int main() {
    SuffixTree tree;
    // Обязательно добавляем уникальный символ конца строки ($), 
    // чтобы все суффиксы стали явными листьями (как учит Гасфилд).
    string s = "xabxa$"; 
    
    tree.build_O_N3(s);
    tree.printTree();

    return 0;
}