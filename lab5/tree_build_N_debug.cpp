#include <iostream>
#include <string>
#include <map>

using namespace std;

int leafEnd = -1;

struct Node {
    map<char, struct Edge*> edges;
    Node* suffix_link;
    int suffix_index;
    int id; // Уникальный номер узла для отладки

    Node(int id) : suffix_link(nullptr), suffix_index(-1), id(id) {}
};

struct Edge {
    int start;
    int* end;
    Node* target;

    Edge(int s, int* e, Node* t) : start(s), end(e), target(t) {}
    
    int length() {
        return *end - start + 1;
    }
};

class SuffixTree {
private:
    Node* root;
    string text;

    Node* active_node;
    int active_edge;
    int active_length;
    int remainingSuffixCount;
    int nodeCounter; // Счетчик для выдачи id узлам

    bool walk_down(Edge* edge) {
        if (active_length >= edge->length()) {
            active_edge += edge->length();
            active_length -= edge->length();
            active_node = edge->target;
            return true;
        }
        return false;
    }

    void extend(int i) {
        leafEnd = i;
        remainingSuffixCount++;
        Node* last_created_internal_node = nullptr;

        while (remainingSuffixCount > 0) {
            if (active_length == 0) {
                active_edge = i;
            }

            char edge_first_char = text[active_edge];
            
            if (active_node->edges.find(edge_first_char) == active_node->edges.end()) {
                Node* new_leaf = new Node(nodeCounter++);
                active_node->edges[edge_first_char] = new Edge(i, &leafEnd, new_leaf);
                
                if (last_created_internal_node != nullptr) {
                    last_created_internal_node->suffix_link = active_node;
                    last_created_internal_node = nullptr;
                }
            } 
            else {
                Edge* edge = active_node->edges[edge_first_char];
                
                if (walk_down(edge)) {
                    continue;
                }

                if (text[edge->start + active_length] == text[i]) {
                    active_length++;
                    if (last_created_internal_node != nullptr && active_node != root) {
                        last_created_internal_node->suffix_link = active_node;
                    }
                    break; // Правило 3 - остановка
                }

                Node* split_node = new Node(nodeCounter++);
                int* split_end = new int(edge->start + active_length - 1);
                
                Edge* old_edge_rem = new Edge(edge->start + active_length, edge->end, edge->target);
                split_node->edges[text[edge->start + active_length]] = old_edge_rem;
                
                edge->end = split_end;
                edge->target = split_node;

                Node* new_leaf = new Node(nodeCounter++);
                split_node->edges[text[i]] = new Edge(i, &leafEnd, new_leaf);

                if (last_created_internal_node != nullptr) {
                    last_created_internal_node->suffix_link = split_node;
                }
                last_created_internal_node = split_node;
            }

            remainingSuffixCount--;

            if (active_node == root && active_length > 0) {
                active_length--;
                active_edge = i - remainingSuffixCount + 1;
            } else if (active_node != root) {
                active_node = active_node->suffix_link ? active_node->suffix_link : root;
            }
        }
    }

    void setSuffixIndices(Node* n, int labelHeight) {
        if (n == nullptr) return;
        bool isLeaf = true;
        for (auto& pair : n->edges) {
            isLeaf = false;
            setSuffixIndices(pair.second->target, labelHeight + pair.second->length());
        }
        if (isLeaf) n->suffix_index = text.length() - labelHeight;
    }

    // flag is_intermediate позволяет выводить дерево до финальной разметки индексов
    void print(Node* node, int depth, bool is_intermediate = false) {
        for (auto& pair : node->edges) {
            Edge* edge = pair.second;
            for (int i = 0; i < depth; ++i) cout << "  ";
            
            int end_idx = *(edge->end);
            string edge_str = text.substr(edge->start, end_idx - edge->start + 1);
            cout << "--(" << edge_str << ")--> ";
            
            // Если у узла нет детей - это лист
            if (edge->target->edges.empty()) {
                if (is_intermediate) cout << "[Leaf ID:" << edge->target->id << "]\n";
                else cout << "[Leaf " << edge->target->suffix_index << "]\n";
            } else {
                cout << "[Node " << edge->target->id;
                // Выводим суффиксную ссылку, если она есть и не ведет в корень (для наглядности)
                if (edge->target->suffix_link && edge->target->suffix_link != root) {
                    cout << " -> link to Node " << edge->target->suffix_link->id;
                }
                cout << "]\n";
                print(edge->target, depth + 1, is_intermediate);
            }
        }
    }

    void printPhase(int phase) {
        cout << "\n=========================================\n";
        cout << "Фаза " << phase << ": Вставляем символ '" << text[phase] << "'\n";
        cout << "-----------------------------------------\n";
        cout << "[Состояние после фазы]\n";
        cout << "Активная точка: { Узел: " << active_node->id 
             << ", Ребро: '" << (active_length > 0 ? string(1, text[active_edge]) : "-") 
             << "', Длина: " << active_length << " }\n";
        cout << "Счетчик rem: " << remainingSuffixCount << "\n";
        cout << "Дерево:\n";
        print(root, 0, true);
    }

public:
    SuffixTree() {
        nodeCounter = 0;
        root = new Node(nodeCounter++); // Корень всегда ID 0
        active_node = root;
        active_edge = -1;
        active_length = 0;
        remainingSuffixCount = 0;
    }

    void build_O_N(string s) {
        text = s;
        cout << "Начинаем построение для строки: " << text << "\n";
        for (int i = 0; i < text.length(); ++i) {
            extend(i);
            printPhase(i); // <-- Тот самый вывод на каждой итерации
        }
        
        setSuffixIndices(root, 0);
        cout << "\n=========================================\n";
        cout << "ФИНАЛЬНОЕ ДЕРЕВО (с индексами суффиксов):\n";
        print(root, 0, false);
    }
};

int main() {
    SuffixTree tree;
    // string s = "xabxa#"; 
    string s = "abacabad#"; 

    tree.build_O_N(s);

    return 0;
}