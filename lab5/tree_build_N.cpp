#include <iostream>
#include <string>
#include <map>

using namespace std;

const int INF = 1e9;

// Глобальный указатель для листьев (тот самый "e" по Гасфилду)
int leafEnd = -1;

struct Node {
    map<char, struct Edge*> edges;
    Node* suffix_link;
    int suffix_index;

    Node() : suffix_link(nullptr), suffix_index(-1) {}
};

struct Edge {
    int start;
    int* end;   // Теперь это указатель! Листья будут указывать на глобальный leafEnd
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

    // --- Active Point ---
    Node* active_node;
    int active_edge; // Индекс символа в text, с которого начинается активное ребро
    int active_length;
    
    int remainingSuffixCount; // Сколько суффиксов ждет добавления в дерево

    // Тот самый Skip/Count trick, оформленный в функцию
    // Возвращает true, если мы перепрыгнули ребро и изменили active_node
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
        // Правило 1: Увеличиваем глобальный конец. ВСЕ листья растут за O(1)
        leafEnd = i;
        remainingSuffixCount++; // Добавился новый суффикс, который нужно вставить
        
        Node* last_created_internal_node = nullptr;

        // Пока у нас есть не вставленные суффиксы
        while (remainingSuffixCount > 0) {
            
            if (active_length == 0) {
                active_edge = i; // Ищем совпадение с текущего символа
            }

            char edge_first_char = text[active_edge];
            
            // Если ребра нет — ПРАВИЛО 2 (создаем новый лист)
            if (active_node->edges.find(edge_first_char) == active_node->edges.end()) {
                Node* new_leaf = new Node();
                // У листа конец указывает на глобальный leafEnd!
                active_node->edges[edge_first_char] = new Edge(i, &leafEnd, new_leaf);
                
                // Если был создан внутренний узел на прошлом шаге, ставим ссылку на текущий active_node
                if (last_created_internal_node != nullptr) {
                    last_created_internal_node->suffix_link = active_node;
                    last_created_internal_node = nullptr;
                }
            } 
            else {
                // Ребро есть. Достаем его.
                Edge* edge = active_node->edges[edge_first_char];
                
                // Пробуем спуститься вниз (Skip/Count)
                if (walk_down(edge)) {
                    continue; // Если спустились, начинаем цикл заново с нового active_node
                }

                // Проверяем следующий символ на ребре — ПРАВИЛО 3 (Showstopper)
                if (text[edge->start + active_length] == text[i]) {
                    active_length++;
                    // Создаем суффиксную ссылку для ранее созданного узла, если он был
                    if (last_created_internal_node != nullptr && active_node != root) {
                        last_created_internal_node->suffix_link = active_node;
                    }
                    // Правило 3 сработало - ОСТАНАВЛИВАЕМ ФАЗУ!
                    break;
                }

                // Символ не совпал — ПРАВИЛО 2 (Разбиваем ребро)
                Node* split_node = new Node();
                int* split_end = new int(edge->start + active_length - 1);
                
                // Старое ребро меняет конец и указывает на split_node
                Edge* old_edge_rem = new Edge(edge->start + active_length, edge->end, edge->target);
                split_node->edges[text[edge->start + active_length]] = old_edge_rem;
                
                edge->end = split_end;
                edge->target = split_node;

                // Создаем новый лист для нового символа
                Node* new_leaf = new Node();
                split_node->edges[text[i]] = new Edge(i, &leafEnd, new_leaf);

                // Работа с суффиксными ссылками
                if (last_created_internal_node != nullptr) {
                    last_created_internal_node->suffix_link = split_node;
                }
                last_created_internal_node = split_node;
            }

            // Суффикс вставлен, уменьшаем счетчик
            remainingSuffixCount--;

            // Обновление Active Point перед следующей итерацией (суффиксные ссылки в действии)
            if (active_node == root && active_length > 0) {
                active_length--;
                active_edge = i - remainingSuffixCount + 1;
            } else if (active_node != root) {
                active_node = active_node->suffix_link ? active_node->suffix_link : root;
            }
        }
    }

    // Вспомогательная функция для простановки индексов листьям (DFS)
    void setSuffixIndices(Node* n, int labelHeight) {
        if (n == nullptr) return;
        
        bool isLeaf = true;
        for (auto& pair : n->edges) {
            isLeaf = false;
            Edge* edge = pair.second;
            setSuffixIndices(edge->target, labelHeight + edge->length());
        }
        
        if (isLeaf) {
            n->suffix_index = text.length() - labelHeight;
        }
    }

    void print(Node* node, int depth) {
        for (auto& pair : node->edges) {
            Edge* edge = pair.second;
            for (int i = 0; i < depth; ++i) cout << "  ";
            
            // Если конец ребра - это leafEnd, выводим актуальное значение
            int end_idx = *(edge->end);
            string edge_str = text.substr(edge->start, end_idx - edge->start + 1);
            cout << "--(" << edge_str << ")--> ";
            
            if (edge->target->suffix_index != -1) 
                cout << "[Leaf " << edge->target->suffix_index << "]\n";
            else {
                cout << "[Node]\n";
            }
            print(edge->target, depth + 1);
        }
    }

public:
    SuffixTree() {
        root = new Node();
        active_node = root;
        active_edge = -1;
        active_length = 0;
        remainingSuffixCount = 0;
    }

    void build_O_N(string s) {
        text = s;
        for (int i = 0; i < text.length(); ++i) {
            extend(i);
        }
        // В самом конце один раз обходим дерево, чтобы проставить суффиксные индексы листьям
        setSuffixIndices(root, 0);
    }

    void printTree() {
        cout << "O(N) Suffix Tree for: " << text << "\n";
        print(root, 0);
    }
};

int main() {
    SuffixTree tree;
    string s = "abacabad#"; 
    
    tree.build_O_N(s);
    tree.printTree();

    return 0;
}