/*
Привет, что ты знаешь об суффиксных деревьях? Умеешь строить их с помощью алгоритма Укконена за N^3? А умеешь переходить к N? Давай пройдёмся по теории по Гасфилду, где разбираются эти темы. Я бы хотел научиться строить суффиксные деревья алгоритмом Укконена сначала просто в лоб, потом строкам с ссылками перехода, а потом уже через промежутки строки и ссылки перехода.
Т.к. это алгоритмическая задача, то код естественно будем реализовывать на C++. Все инструкции компиляции пишутся на arch linux, исполянемый файл должен называться <имя>.o или <имя>.out
Давай сейчас все реализации модифицировать в tree_build.cpp
*/

/*
В твоей наивной реализации не учтено то, что суффикс одной подстроки может оказаться префиксом другой. В этом случае мы теряем целое решение. Может стоит добавить терминальный символ? Пусть у нас это будет "#".
Или ты считаешь, что в наивной реализации это излишество?
Также есть вопрос по применению правила 3 вот тут:
```
                        Node* split_node = new Node(false);
                        current->edges.erase(edge_str); // Удаляем старое ребро
                        current->edges[edge_str.substr(0, match_len)] = split_node; // Ребро до разрыва
                        split_node->edges[rem_edge] = next_node; // Остаток старого ребра
                        
                        // Создаем новый лист для нового символа
                        Node* new_leaf = new Node(true);
                        new_leaf->suffix_index = j;
                        split_node->edges[string(1, next_char)] = new_leaf;
                        
                        return;
```
Получается мы разбиваем текущее ребро на два (та часть которая нам нужна для новой подстроки и та часть, которая остаётся до неё). Далее из текущей мапы "потомков" мы удалеям ту строку, которую сейчас будем резать. Добавляем в текущую мапу начало разрезанной строки, а к этой разренной строке добавляем в мапу потомоков остаток разрезанной строки с указателем на все оставшиеся ноды, которые шли за этой строкой.
Дальше создаётся новая нода, для найденого остатка строки, у неё мы увеличиваем индекс. Ну и в мапу остатков закидываем остаток только вставляемой строки начиная с 1 символа, т.к. нулевой ещё совпадает с актуальной строкой.
Всё так?
*/

/*
Теперь я не понял, как работает это цикл, точнее, зачем он нужен?
```cpp
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
```
Получается для подстроки мы идём от корня и cur_j начинаем с j. j в нашем случае левый конец, i - правый. Пока левый не вышел за границы правого берём char из текста по нужному индексы. Если чудом смогли найти чар, который является концом подстроки, то выходим. Иначе ищем edge соответсвующий выбранному char из текста. Находим длину этого отрезка, а также длину отрезка между правым и левым указателями строки. Если эти величина больше или равны друг другу, то мы можем переходить к общению с потомком, т.к. данный отрезок строки был пройден, и текущий левый индекс сдвигаем на всю длину пройденного участка. Если же мы попали в ситуацию с остановкой где-то внутри активного ребра, то фиксируем активную ноду, индекс её начала, её длину, и почему-то сдвигаем левый конец в самое правое доступное значение. Вот этот момент мне совсем не понятне. Далее у нас вообще стоит выход из цикла через break, НО зачем он, после установки cur_j = i, ведь while cur_j < i больше не выполнится. Итого из этого цикла мы всегда выдем с cur_j >= i.
Получается мы получили cur_j >= i, но почему-то ты утверждаешь дальше "// Теперь мы нашли место вставки.current_j == i.". А что будет, если больше?
В целом больше вопросов нет к N^2 реализации.
*/


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
    int active_edge;
    int active_length;
    int remainingSuffixCount;

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
                Node* new_leaf = new Node();
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
                    break;
                }

                Node* split_node = new Node();
                int* split_end = new int(edge->start + active_length - 1);
                
                Edge* old_edge_rem = new Edge(edge->start + active_length, edge->end, edge->target);
                split_node->edges[text[edge->start + active_length]] = old_edge_rem;
                
                edge->end = split_end;
                edge->target = split_node;

                Node* new_leaf = new Node();
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
            Edge* edge = pair.second;
            setSuffixIndices(edge->target, labelHeight + edge->length());
        }
        
        if (isLeaf) {
            n->suffix_index = text.length() - labelHeight;
        }
    }

public:
    SuffixTree() {
        root = new Node();
        active_node = root;
        active_edge = -1;
        active_length = 0;
        remainingSuffixCount = 0;
        leafEnd = -1; // Сбрасываем на случай множественных тестов
    }

    void build_O_N(string s) {
        text = s;
        for (int i = 0; i < text.length(); ++i) {
            extend(i);
        }
        setSuffixIndices(root, 0);
    }

    // НОВЫЙ МЕТОД: Поиск минимального циклического сдвига
    string getSmallestCyclicShift(int original_length) {
        string result = "";
        Node* current = root;
        int length_needed = original_length;

        // Пока не собрали строку нужной длины
        while (length_needed > 0) {
            Edge* best_edge = nullptr;

            // Ищем ребро, начинающееся с лексикографически минимального символа
            for (auto& pair : current->edges) {
                if (pair.first == '#') continue; 
                
                best_edge = pair.second;
                break;
            }

            if (best_edge == nullptr) break; // Защита (в теории никогда не сработает)

            // Актуальная длина ребра с учетом глобального указателя листьев
            int edge_len = *(best_edge->end) - best_edge->start + 1;

            if (edge_len >= length_needed) {
                // Если на ребре символов больше или равно тому, что нам осталось
                result += text.substr(best_edge->start, length_needed);
                length_needed = 0;
            } else {
                // Если ребро короче, берем его целиком и идем в следующий узел
                result += text.substr(best_edge->start, edge_len);
                length_needed -= edge_len;
                current = best_edge->target;
            }
        }

        return result;
    }
};

int main() {
    // Оптимизация ввода-вывода для олимпиадных задач
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string s;
    // Читаем до конца файла (вдруг в тестирующей системе несколько тестов)
    while (cin >> s) {
        int L = s.length();
        
        // Для поиска циклического сдвига удваиваем строку и добавляем терминальный символ
        string tree_string = s + s + "#";
        
        SuffixTree tree;
        tree.build_O_N(tree_string);
        
        // Ищем и выводим минимальный сдвиг исходной длины
        cout << tree.getSmallestCyclicShift(L) << "\n";
    }

    return 0;
}