#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

const int INF = 1e9;


struct Node {
    int start;
    int end;
    int link; 
    map<char, int> next;

    int length(int leafEnd) {
        return (end == INF ? leafEnd : end) - start + 1;
    }
};

class SuffixTree {
private:
    vector<Node> tree; 
    string text;
    
    int root;
    int active_node;
    int active_edge;
    int active_length;
    int remainingSuffixCount;
    int leafEnd;

    int newNode(int start, int end) {
        tree.emplace_back({start, end, 0, {}}); // link по умолчанию 0
        return tree.size() - 1;
    }

    bool walk_down(int child_idx) {
        int len = tree[child_idx].length(leafEnd);
        if (active_length >= len) {
            active_edge += len;
            active_length -= len;
            active_node = child_idx;
            return true;
        }
        return false;
    }

    void extend(int i) {
        leafEnd = i;
        remainingSuffixCount++;
        int last_created_internal_node = -1;

        while (remainingSuffixCount > 0) {
            if (active_length == 0) {
                active_edge = i;
            }

            char edge_char = text[active_edge];

            // правило 2 
            if (tree[active_node].next.find(edge_char) == tree[active_node].next.end()) {
                tree[active_node].next[edge_char] = newNode(i, INF);

                if (last_created_internal_node != -1) {
                    tree[last_created_internal_node].link = active_node;
                    last_created_internal_node = -1;
                }
            } 
            else {
                int child_idx = tree[active_node].next[edge_char];

                if (walk_down(child_idx)) continue;

                // правило 3
                if (text[tree[child_idx].start + active_length] == text[i]) {
                    active_length++;
                    if (last_created_internal_node != -1 && active_node != root) {
                        tree[last_created_internal_node].link = active_node;
                    }
                    break;
                }

                // правило 2
                int split_node = newNode(tree[child_idx].start, tree[child_idx].start + active_length - 1);
                tree[active_node].next[edge_char] = split_node;
                
                tree[child_idx].start += active_length;
                tree[split_node].next[text[tree[child_idx].start]] = child_idx;
                
                tree[split_node].next[text[i]] = newNode(i, INF);

                if (last_created_internal_node != -1) {
                    tree[last_created_internal_node].link = split_node;
                }
                last_created_internal_node = split_node;
            }

            remainingSuffixCount--;

            if (active_node == root && active_length > 0) {
                active_length--;
                active_edge = i - remainingSuffixCount + 1;
            } else if (active_node != root) {
                active_node = tree[active_node].link; 
            }
        }
    }

public:
    SuffixTree(const string& s) {
        text = s;
        // количество узлов в суффиксном дереве <= 2*N
        tree.reserve(text.length() * 2 + 10); 
        
        root = newNode(-1, -1);
        active_node = root;
        active_edge = -1;
        active_length = 0;
        remainingSuffixCount = 0;
        leafEnd = -1;
        
        for (int i = 0; i < text.length(); ++i) {
            extend(i);
        }
    }

    string getSmallestCyclicShift(int original_length) {
        string result = "";
        int current = root;
        int length_needed = original_length;

        while (length_needed > 0) {
            int best_child = -1;

            for (auto& pair : tree[current].next) {
                if (pair.first == '#') continue; 
                best_child = pair.second;
                break;
            }

            if (best_child == -1) break;

            int edge_len = tree[best_child].length(leafEnd);

            if (edge_len >= length_needed) {
                result += text.substr(tree[best_child].start, length_needed);
                length_needed = 0;
            } else {
                result += text.substr(tree[best_child].start, edge_len);
                length_needed -= edge_len;
                current = best_child;
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
        string tree_string = s + s + "#";
        SuffixTree tree(tree_string);
        cout << tree.getSmallestCyclicShift(s.length()) << "\n";
    }

    return 0;
}