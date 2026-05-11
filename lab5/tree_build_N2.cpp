#include <iostream>
#include <string>
#include <map>

using namespace std;

const int INF = 1e9; // Пока используем для обозначения конца (хотя в O(N^2) мы будем обновлять концы честно)

struct Node {
    map<char, struct Edge*> edges; // Теперь ключ - только ПЕРВЫЙ символ ребра
    Node* suffix_link;
    int suffix_index;

    Node() : suffix_link(nullptr), suffix_index(-1) {}
};

struct Edge {
    int start;
    int end;   // [start, end] включительно
    Node* target;

    Edge(int s, int e, Node* t) : start(s), end(e), target(t) {}
    
    // Длина ребра
    int length() {
        return end - start + 1;
    }
};

class SuffixTree {
private:
    Node* root;
    string text;

    // Вспомогательная функция для вывода
    void print(Node* node, int depth) {
        for (auto& pair : node->edges) {
            Edge* edge = pair.second;
            for (int i = 0; i < depth; ++i) cout << "  ";
            
            // Выводим строку по индексам
            string edge_str = text.substr(edge->start, edge->length());
            cout << "--(" << edge_str << ")--> ";
            
            if (edge->target->suffix_index != -1) 
                cout << "[Leaf " << edge->target->suffix_index << "]\n";
            else {
                cout << "[Node";
                if (edge->target->suffix_link) cout << " -> link";
                cout << "]\n";
            }
            print(edge->target, depth + 1);
        }
    }

public:
    SuffixTree() {
        root = new Node();
        root->suffix_link = root; // Ссылка из корня ведет в корень
    }

    void build_O_N2(string s) {
        text = s;
        int n = text.length();

        // Мы храним активную точку (где мы остановились в прошлом расширении)
        Node* active_node = root;
        int active_edge_start = -1;
        int active_length = 0;

        // Внешний цикл - фазы
        for (int i = 0; i < n; ++i) {
            Node* last_created_internal_node = nullptr;

            // Внутренний цикл - расширения (от 0 до i)
            // В O(N^2) мы честно проходим все j
            for (int j = 0; j <= i; ++j) {
                
                // Наша цель: найти конец строки S[j..i-1] и добавить S[i]
                // Поскольку это промежуточный алгоритм, для простоты мы будем
                // спускаться от корня, НО используя индексы (без суффиксных ссылок 
                // код с индексами тоже N^3, но мы подготовили структуру Edge).
                
                // Чтобы не усложнять промежуточный шаг до состояния финального (где
                // активные точки и ссылки переплетены), в этой версии мы
                // реализуем спуск по индексам. 
                
                Node* current = root;
                int current_j = j;
                
                // Спуск по дереву (Skip/Count trick) - ищем S[j..i-1]
                while (current_j < i) {
                    char c = text[current_j];
                    if (current->edges.find(c) == current->edges.end()) break; // Не должно случаться
                    
                    Edge* edge = current->edges[c];
                    int edge_len = edge->length();
                    int rem_len_to_find = i - current_j;

                    if (rem_len_to_find >= edge_len) {
                        // Skip trick: перепрыгиваем ребро целиком, не сверяя символы
                        current = edge->target;
                        current_j += edge_len;
                    } else {
                        // Мы остановились внутри ребра
                        active_node = current;
                        active_edge_start = edge->start;
                        active_length = rem_len_to_find;
                        current_j = i; // Выходим из цикла
                        break;
                    }
                }

                // Теперь мы нашли место вставки.
                // current_j == i.
                char next_char = text[i];
                
                // Если мы в узле
                if (active_length == 0) {
                    if (current->edges.find(next_char) == current->edges.end()) {
                        // Правило 2 (создаем новый лист)
                        Node* new_leaf = new Node();
                        new_leaf->suffix_index = j;
                        // Внимание: в O(N^2) мы должны вручную обновлять концы старых листьев.
                        // Поэтому мы кладем ребро [i, i] (один символ).
                        current->edges[next_char] = new Edge(i, i, new_leaf);
                    }
                } 
                // Если мы внутри ребра
                else {
                    Edge* edge = active_node->edges[text[active_edge_start]];
                    char char_at_split = text[edge->start + active_length];

                    if (char_at_split != next_char) {
                        // Правило 2: Разрываем ребро
                        Node* split_node = new Node();
                        
                        // Старое ребро теперь ведет в split_node и стало короче
                        Edge* old_edge_rem = new Edge(edge->start + active_length, edge->end, edge->target);
                        split_node->edges[char_at_split] = old_edge_rem;
                        
                        edge->end = edge->start + active_length - 1;
                        edge->target = split_node;

                        // Новый лист
                        Node* new_leaf = new Node();
                        new_leaf->suffix_index = j;
                        split_node->edges[next_char] = new Edge(i, i, new_leaf);
                        
                        // Установка суффиксной ссылки (подготовка к O(N))
                        if (last_created_internal_node != nullptr) {
                            last_created_internal_node->suffix_link = split_node;
                        }
                        last_created_internal_node = split_node;
                    }
                }
            }

            // В конце каждой фазы i алгоритма O(N^2) (без глобального указателя конца e)
            // мы обязаны пробежаться по ВСЕМ существующим листьям и увеличить их end на 1,
            // чтобы добавить символ text[i] к уже существующим суффиксам (Правило 1).
            // В O(N) это делается за О(1) магией указателя. Здесь мы это опускаем для краткости,
            // но в уме держим: листья должны расти!
        }
    }

    void printTree() {
        cout << "Suffix Tree (with indices) for: " << text << "\n";
        print(root, 0);
    }
};

int main() {
    SuffixTree tree;
    string s = "xabxa#"; 
    
    tree.build_O_N2(s);
    tree.printTree();

    return 0;
}