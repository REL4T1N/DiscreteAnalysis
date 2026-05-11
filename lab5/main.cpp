#include <iostream>
#include <string>
#include <map>

using namespace std;

const int INF = 1e9;

int leafEnd = -1;

struct Node {
    map<char, struct Edge*> edges;
    Node* suffix_link;
    int suffix_index;

    Node() : suffix_link(nullptr), suffix_index(-1) {}
};

struct Edge {
    int start;
    int* end;   // листья должны указывать на глобальный leafEnd
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

    // начальное состояние active point 
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
        leafEnd = -1;
    }

    void build_O_N(string s) {
        text = s;
        for (int i = 0; i < text.length(); ++i) {
            extend(i);
        }
        setSuffixIndices(root, 0);
    }

    string getSmallestCyclicShift(int original_length) {
        string result = "";
        Node* current = root;
        int length_needed = original_length;

        while (length_needed > 0) {
            Edge* best_edge = nullptr;

            for (auto& pair : current->edges) {
                if (pair.first == '#') continue; 
                
                best_edge = pair.second;
                break;
            }

            if (best_edge == nullptr) break;

            int edge_len = *(best_edge->end) - best_edge->start + 1;

            if (edge_len >= length_needed) {
                result += text.substr(best_edge->start, length_needed);
                length_needed = 0;
            } else {
                result += text.substr(best_edge->start, edge_len);
                length_needed -= edge_len;
                current = best_edge->target;
            }
        }

        return result;
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string s;
    while (cin >> s) {
        int L = s.length();
        
        string tree_string = s + s + "#"; // # - терминальный символ
        
        SuffixTree tree;
        tree.build_O_N(tree_string);
        
        cout << tree.getSmallestCyclicShift(L) << "\n";
    }

    return 0;
}